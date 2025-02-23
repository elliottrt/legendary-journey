#ifndef _KERNEL_VIRTUALMEMORY
#define _KERNEL_VIRTUALMEMORY

#include "common/types.h"

// See https://wiki.osdev.org/Setting_Up_Paging
// See https://wiki.osdev.org/Paging

#define KERNEL_MEM_END PHYSTOP

extern char etext[];
extern char data[];
extern char edata[];
extern char end[];

void kpginit(void);

// TODO: do something that doesn't involve exposing kpgdir
extern uint32_t *kpgdir;

// returns true if the page is present in the page directory
bool pg_is_present(uint32_t *pgdir, uintptr_t ptr);

// maps 'page_count' pages starting at 'virt_addr' with the given permissions
bool pg_map_range(uint32_t *pgdir, uintptr_t virt_addr, size_t page_count, int perm);

// unmaps 'page_count' pages starting at 'virt_addr'
void pg_unmap_range(uint32_t *pgdir, uintptr_t virt_addr, size_t page_count);

#endif
