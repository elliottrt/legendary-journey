#ifndef _KERNEL_TIMER
#define _KERNEL_TIMER

#include "common/types.h"

// must be higher than 18
#define TIMER_HZ 1000

void timerinit(void);

extern uint32_t _timer_ticks;
#define timerget() (_timer_ticks)

// wait until timerget() is at least time
void timer_wait_until(uint32_t time);

// wait for a number of milliseconds
void timer_wait(uint32_t ms);

#endif
