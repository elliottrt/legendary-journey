#include "virtmem.h"
#include "types.h"
#include "x86.h"
#include "graphics/printf.h"
#include "std.h"
#include "kalloc.h"
#include "mmu.h"

// This table defines the kernel's mappings, which are present in
// every process's page table.
struct kmap {
  uintptr_t virt;
  uintptr_t phystart;
  uintptr_t phyend;
  int perm;
}; 

/* MEMORY MAP

physical range                  -> virtual range                            <what's in this range>

0               - EXTMEM        -> KERNBASE         - KERNBASE+EXTMEM       <bios + io code>
EXTMEM          - phys(data)    -> KERNBASE+EXTMEM  - data                  <kernel code + rodata>
phys(data)      - PHYSTOP/2     -> data             - PHYSTOP/2             <kernel data + extra space>
PHYSTOP/2       - PHYSTOP       -> USERBASE         - <has no end?>         <user program space>

*/

static struct kmap kmap[] = {
 	{ (uintptr_t) KERNBASE,    0,                    EXTMEM,                 PTE_W}, // I/O space + BIOS stuff
 	{ (uintptr_t) KERNLINK,    EXTMEM,               V2P(data),              0},     // kern text+rodata
 	{ (uintptr_t) data,        V2P(data),            0/*set by kpginit*/,    PTE_W}, // kern data+memory
    { (uintptr_t) USERBASE,    0/*set by kpginit*/,  0/*set by kpginit*/,    PTE_W}, // user code location
};

uint32_t *kpgdir;

uint32_t entrypgdir[PDENTRIES] __attribute__ ((aligned(PGSIZE))) = {
  // Map VA's [0, 4MB) to PA's [0, 4MB)
  [0] = (0) | PTE_P | PTE_W | PTE_PS,
  // Map VA's [KERNBASE, KERNBASE+4MB) to PA's [0, 4MB)
  [KERNBASE>>PDXSHIFT] = (0) | PTE_P | PTE_W | PTE_PS,
};

static uint32_t *walkpgdir(uint32_t *pgdir, const void *va, int alloc) {
    uint32_t *pde, *pgtab;

    // ptr to position in the page table representing va
    pde = &pgdir[PDX(va)];

    // if already present, the page table is set to the 
    // value found in the page table
    if(*pde & PTE_P) {

        pgtab = (uint32_t *) P2V(PTE_ADDR(*pde));

    // otherwise,
    } else {

        // we allocate a new page table if one isn't available
        if(!alloc || (pgtab = (uint32_t *) kalloc()) == 0)
            return 0;

        // clear all present bits and make sure there's no interference
        memset(pgtab, 0, PGSIZE);
        
        // set the directory entry representing va to the physical address
        // of the newly created page table, and set present, write, and user permissions
        *pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;

    }

    // return the address of the page table entry representing va
    return &pgtab[PTX(va)];
}

static int mappages(uint32_t *pgdir, void *va, uint32_t size, uint32_t pa, int perm) {

    // get virtual addresses of first and last page to be mapped
    void *a = (void *) PGROUNDDOWN((uint32_t) va);
    void *last = (void *) PGROUNDDOWN(((uint32_t) va) + size - 1);

    uint32_t *pte;

    // for each page to be entered into the page table,
    for(;;) {

        // get the address of the page table entry representing va
        if ((pte = walkpgdir(pgdir, a, true)) == 0)
            return -1;

        // if it's already present, something's gone wrong
        if (*pte & PTE_P) {
            printf("kmap: error: unable to remap pages: entry for 0x%8x already present\n", a);
            STOP();
        }

        // set its physical address, permissions, and present flag
        *pte = pa | perm | PTE_P;

        if (a == last)
            break;

        a += PGSIZE;
        pa += PGSIZE;
    }
    return 0;
}

inline void kpgupdate(uint32_t *dir) {
    lcr3(V2P(dir));
}

void kpginit(void) {

    kmap[2].phyend = KERNEL_MEM_END;
    kmap[3].phystart = KERNEL_MEM_END;
    kmap[3].phyend = PHYSTOP;

    kpgdir = (uint32_t *) kalloc();
    memset(kpgdir, 0, PGSIZE);

    for (struct kmap *k = kmap; k < &kmap[NELEM(kmap)]; k++) {
        uint32_t region_size = k->phyend - k->phystart;

        printf("kmap: 0x%8x - 0x%8x to 0x%8x - 0x%8x\n", k->phystart, k->phyend, k->virt, k->virt + region_size);

        int success = mappages(kpgdir, (void *) k->virt, region_size, k->phystart, k->perm);
        if (success < 0) {
            printf("kmap: error: unable to map pgdir: %s\n", strerror(errno));
            STOP();
        }
    }

    kpgupdate(kpgdir);
}

bool pg_is_present(uint32_t *pgdir, uintptr_t ptr) {
    // get entry in the page table for this page
    uint32_t *pte = walkpgdir(pgdir, (void *) PGROUNDDOWN(ptr), false);

    // return whether the page table exists and the page is present
    return pte && (*pte & PTE_P);
}

bool pg_map(uint32_t *pgdir, uintptr_t page_addr, uintptr_t virt_addr, int perm) {
    // get entry in the page table for this virtual address
    uint32_t *pte = walkpgdir(pgdir, (void *) PGROUNDDOWN(virt_addr), true);

    // if virt_addr already taken, fail
    if (page_addr != 0 && *pte & PTE_P) return false;

    // insert page_addr at virt_addr
    *pte = PGROUNDDOWN(page_addr) | perm | (page_addr != 0);

    return true;
}

bool pg_map_range(uint32_t *pgdir, uintptr_t virt_addr, size_t page_count, int perm) {
    uint32_t *pte;

    // ensure that all virtual addresses are not present
    for (size_t i = 0; i < page_count; i++) {
        pte = walkpgdir(pgdir, (void *) PGROUNDDOWN(virt_addr + i * PGSIZE), true);

        if (!pte || (*pte & PTE_P)) return false;
    }

    // allocate pages and map them
    for (size_t i = 0; i < page_count; i++) {
        uintptr_t new_page = (uintptr_t) kalloc();

        if (!new_page || !pg_map(pgdir, V2P_WO(new_page), virt_addr + i * PGSIZE, perm)) 
            return false;
    }

    return true;
}

void pg_unmap_range(uint32_t *pgdir, uintptr_t virt_addr, size_t page_count) {
    // deallocate pages and demap them
    for (size_t i = 0; i < page_count; i++) {
        uint32_t *pte = walkpgdir(pgdir, (void *)(virt_addr + i * PGSIZE), false);

        if (pte) {
            kfree(P2V(PTE_ADDR(*pte)));

            // clear page address, permissions, and present
            *pte = 0;
        }
    }
}
