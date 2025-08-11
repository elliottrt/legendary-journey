#ifndef _KERNEL_KALLOC
#define _KERNEL_KALLOC

#include "common/types.h"
#include "kernel/memory/malloc.h"

extern uint32_t PHYSTOP;
extern rm_ctx_t *rm_global_ctx;

void *kalloc(void);
void kfree(void *address);

void kallocinit(void);
void kallocexpand(void);

uint32_t kallocavailable(void);
uint32_t kalloctotal(void);

#endif
