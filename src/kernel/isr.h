#ifndef _KERNEL_ISR
#define _KERNEL_ISR

#include "types.h"

#define NUM_ISRS 256

typedef void (*isrhandler_t)(struct registers *);

void isrinstall(uchar i, isrhandler_t handler);
void isrinit();

#endif
