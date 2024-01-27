#include "virtmem.h"
#include "types.h"
#include "x86.h"
#include "printf.h"
#include "std.h"
#include "kalloc.h"
#include "mmu.h"

// This table defines the kernel's mappings, which are present in
// every process's page table.
static struct kmap {
  void *virt;
  uint phystart;
  uint phyend;
  int perm;
} kmap[] = {
 	{ (void*)KERNBASE,    0,              EXTMEM,                 PTE_W}, // I/O space + BIOS stuff
 	{ (void*)KERNLINK,    V2P(KERNLINK),  V2P(data),              0},     // kern text+rodata
 	{ (void*)data,        V2P(data),      0/*set in kpginit*/,    PTE_W}, // kern data+memory
};

uint *kpgdir;

uint entrypgdir[PDENTRIES] __attribute__ ((aligned(PGSIZE))) = {
  // Map VA's [0, 4MB) to PA's [0, 4MB)
  [0] = (0) | PTE_P | PTE_W | PTE_PS,
  // Map VA's [KERNBASE, KERNBASE+4MB) to PA's [0, 4MB)
  [KERNBASE>>PDXSHIFT] = (0) | PTE_P | PTE_W | PTE_PS,
};

static uint *walkpgdir(uint *pgdir, const void *va, int alloc) {
    uint *pde, *pgtab;

    // ptr to position in the page table representing va
    pde = &pgdir[PDX(va)];

    // if already present, the page table is set to the 
    // value found in the page table
    if(*pde & PTE_P) {

        pgtab = (uint *) P2V(PTE_ADDR(*pde));

    // otherwise,
    } else {

        // we allocate a new page table if one isn't available
        if(!alloc || (pgtab = (uint *) kalloc()) == 0)
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

static int mappages(uint *pgdir, void *va, uint size, uint pa, int perm) {

    char *a = (char *) PGROUNDDOWN((uint) va);
    char *last = (char *) PGROUNDDOWN(((uint) va) + size - 1);

    uint *pte;

    // for each page to be entered into the page table,
    for(;;) {

        // get the address of the page table entry representing va
        if ((pte = walkpgdir(pgdir, a, 1)) == 0)
            return -1;

        // if it's already present, something's gone wrong
        if (*pte & PTE_P) {
            printferr();
            printf("error: unable to remap pages\n");
            while(1);
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

extern ushort *screen_addr;

void kpginit(void) {

    kmap[2].phyend = PHYSTOP;

    kpgdir = (uint *) kalloc();

    memset(kpgdir, 0, PGSIZE);

    for (struct kmap *k = kmap; k < &kmap[NELEM(kmap)]; k++) {

        //printf("kmap elem: 0x%p 0x%p 0x%p 0x%x\n", k->virt, k->phystart, k->phyend, k->perm);

        int success = mappages(kpgdir, k->virt, k->phyend - k->phystart, (uint) k->phystart, k->perm);
        if (success < 0) {
            printferr();
            printf("error: unable to map pgdir\n");
            while(1);
        }

    }

    lcr3(V2P(kpgdir));

}
