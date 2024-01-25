#ifndef _KERNEL_TIMER
#define _KERNEL_TIMER

#include "types.h"

void timerinit(uint hz);
ulong timerget(void);
void timersethz(uint hz);

#endif
