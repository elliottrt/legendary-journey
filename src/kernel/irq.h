#ifndef _KERNEL_IRQ
#define _KERNEL_IRQ

#include "types.h"
#include "isr.h"

typedef void (*irqhandler_t)(struct registers *);

void irqenable(uint i);
void irqdisable(uint i);

void irqinit(void);
void irqinstall(uint i, irqhandler_t handler);

#endif
