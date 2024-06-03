#ifndef _KERNEL_IRQ
#define _KERNEL_IRQ

#include "types.h"
#include "isr.h"

typedef void (*irqhandler_t)(struct registers *);

void irqenable(uint32_t i);
void irqdisable(uint32_t i);

void irqinit(void);
void irqinstall(uint32_t i, irqhandler_t handler);

#endif
