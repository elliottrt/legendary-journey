#ifndef _KERNEL_TIMER
#define _KERNEL_TIMER

#include "types.h"

void timerinit(uint hz);
void timersethz(uint hz);

extern ulong _timer_ticks;
#define timerget() (_timer_ticks)

#endif
