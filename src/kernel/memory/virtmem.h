#ifndef _KERNEL_VIRTUALMEMORY
#define _KERNEL_VIRTUALMEMORY

#include "types.h"

// See https://wiki.osdev.org/Setting_Up_Paging
// See https://wiki.osdev.org/Paging

extern uint32_t PHYSTOP;

extern char etext[];
extern char data[];
extern char edata[];
extern char end[];

void kpginit(void);

#endif
