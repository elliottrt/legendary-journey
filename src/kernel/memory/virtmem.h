#ifndef _KERNEL_VIRTUALMEMORY
#define _KERNEL_VIRTUALMEMORY

#include "types.h"

// See https://wiki.osdev.org/Setting_Up_Paging
// See https://wiki.osdev.org/Paging

#define KERNEL_MEM_END (PHYSTOP / 2)

extern char etext[];
extern char data[];
extern char edata[];
extern char end[];

void kpginit(void);

// returns true if the page is present in the page directory
bool pg_is_present(uint32_t *pgdir, uintptr_t ptr);

// maps a given virtual address to a physical page address with the given permissions
// note that this does not check whether that page is already mapped by another virtual address
// if page_addr is NULL, sets the page entry to not present.
bool pg_map(uint32_t *pgdir, uintptr_t page_addr, uintptr_t virt_addr, int perm);

// maps 'page_count' pages starting at 'virt_addr' with the given permissions
bool pg_map_range(uint32_t *pgdir, uintptr_t virt_addr, size_t page_count, int perm);

// unmaps 'page_count' pages starting at 'virt_addr'
void pg_unmap_range(uint32_t *pgdir, uintptr_t virt_addr, size_t page_count);

#endif
