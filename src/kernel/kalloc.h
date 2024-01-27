#ifndef _KERNEL_KALLOC
#define _KERNEL_KALLOC

#include "types.h"

char *kalloc(void);
void kfree(char *address);

void kallocinit(void);
void kallocexpand(void);

uint kallocavailable(void);
uint kalloctotal(void);

#endif
