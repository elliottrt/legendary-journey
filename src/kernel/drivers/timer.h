#ifndef _KERNEL_TIMER
#define _KERNEL_TIMER

#include "common/types.h"

// must be higher than 18
#define TIMER_HZ 100

void timerinit(void);

extern uint64_t _timer_ticks;
#define timerget() (_timer_ticks)

// TODO: do these actually work???
#define timeruntil(t)   while (_timer_ticks < t) {}
#define timerwait(t)    do { uint64_t _finish_time = _timer_ticks + t; timeruntil(_finish_time); } while (0)

#endif
