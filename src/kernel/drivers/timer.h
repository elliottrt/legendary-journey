#ifndef _KERNEL_TIMER
#define _KERNEL_TIMER

#include "types.h"

// must be higher than 18
#define TIMER_HZ 100

void timerinit(void);

extern uint64_t _timer_ticks;
#define timerget() (_timer_ticks)

#define timeruntil(t)   while (_timer_ticks < t) {}
#define timerwait(t)    do { ulong finish = _timer_ticks + t; timeruntil(finish); } while (0)

#endif
