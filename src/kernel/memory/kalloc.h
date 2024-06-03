#ifndef _KERNEL_KALLOC
#define _KERNEL_KALLOC

#include "types.h"

void *kalloc(void);
void kfree(void *address);

void kallocinit(void);
void kallocexpand(void);

uint32_t kallocavailable(void);
uint32_t kalloctotal(void);

#endif
