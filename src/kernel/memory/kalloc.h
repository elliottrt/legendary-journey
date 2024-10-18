#ifndef _KERNEL_KALLOC
#define _KERNEL_KALLOC

#include "types.h"

extern uint32_t PHYSTOP;

void *kalloc(void);
void kfree(void *address);

void kallocinit(void);
void kallocexpand(void);

uint32_t kallocavailable(void);
uint32_t kalloctotal(void);

#endif
