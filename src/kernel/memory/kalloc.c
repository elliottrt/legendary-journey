#include "kernel/memory/kalloc.h"
#include "kernel/memory/virtmem.h"
#include "kernel/memory/malloc.h"
#include "kernel/graphics/printf.h"
#include "common/std.h"
#include "common/mmu.h"

uint32_t PHYSTOP = 0;

extern uint16_t _memregioncount;
extern struct memregion *_memregions;

void read_memory_regions(void);

struct run {
    struct run *next;
};

static struct run *freemem;
static uint32_t available;
static uint32_t total;

rm_ctx_t *rm_global_ctx = NULL;

void *kalloc(void) {
    if (freemem == NULL) {
        errno = ENOMEM;
        return NULL;
    } 

    struct run *allocated = freemem;
    freemem = freemem->next;

    available--;

    return (void *) allocated;
}

void kfree(void *address) {
    if (!address) return;

    if ((uint32_t) address % PGSIZE) {
        printf("error: unaligned address %p to kfree\n", address);
        STOP();
    }

    if (address < (void *) end) {
        printf("error: low invalid address %p to kfree\n", address);
        STOP();
    }

    if (V2P(address) >= KERNEL_MEM_END) {
        printf("error: high invalid address %p to kfree\n", address);
        STOP();
    }

    memset(address, 0x00, PGSIZE);

    struct run *dealloced = address;

    dealloced->next = freemem;
    freemem = dealloced;
    
    available++;
}

void kfreerange(void *start, void *end) {
    int8_t *pstart = (int8_t *) PGROUNDUP((uint32_t) start);
    for(; pstart + PGSIZE <= (int8_t *) end; pstart += PGSIZE, total++) 
        kfree(pstart);
}

void kallocinit(void) {
    // correct memregions to virtual address
    _memregions = P2V(_memregions);
    // calculate PHYSTOP from memory map
    read_memory_regions();

    kfreerange(end, P2V(4*1024*1024));
}

void kallocexpand(void) {
    kfreerange(P2V(4*1024*1024), P2V(KERNEL_MEM_END));

    // give the general allocator its some of pages

    const size_t block_size = MALLOC_ALLOC_PAGES * PGSIZE;
    void *block_end = (uint8_t *) KERNBASE - PGSIZE;
    void *block_start = (uint8_t *) block_end - block_size;

    // we should end the allocation block at KERNBASE
    // but we don't want the risk of overwriting bios info
    // so we stick a non-writable page at the end

    pg_map_range(
        kpgdir,
        (uintptr_t) block_start,
        MALLOC_ALLOC_PAGES,
        PTE_W
    );

    pg_map_range(
        kpgdir,
        (uintptr_t) (KERNBASE - 1 * PGSIZE),
        1,
        0
    );

    rm_global_ctx = rm_init_block(block_start, block_size);

}

uint32_t kallocavailable(void) {
    return available;
}

uint32_t kalloctotal(void) {
    return total;
}

void read_memory_regions(void) {

    for (uint32_t i = 0; i < _memregioncount; i++) {
        struct memregion *region = &_memregions[i];

        // printf("region %d: 0x%x\n", i, region);
        // printf("    addr: 0x%x\n", region->address);
        // printf("    length: %d\n", region->length);
        // printf("    type: %d\n", region->type);

        if (region->address == KERNEL_LOAD && region->type == 1) {
            PHYSTOP = (uint32_t) (KERNEL_LOAD + region->length);
            printf("kernel memory region: 0x%x to 0x%x\n", KERNEL_LOAD, KERNEL_LOAD + region->length);
        }

    }

    if (PHYSTOP == 0) {
        printf("unable to find region of memory beginning at 0x%x\n", KERNEL_LOAD);
        STOP();
    }

}
