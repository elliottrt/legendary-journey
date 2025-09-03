#include "kernel/drivers/timer.h"
#include "kernel/graphics/printf.h"
#include "kernel/cpu/irq.h"
#include "common/x86.h"
#include "common/std.h"

uint32_t _timer_ticks = 0;

void timerinterrupt(struct registers *regs) {
    UNUSED(regs);

	_timer_ticks++;
}

void timersethz(uint32_t hz) {
    uint16_t divisor = 1193180 / hz;    /* Calculate our divisor */
    outb(0x43, 0x36);             	    /* Set our command byte 0x36 */
    outb(0x40, divisor & 0xFF);   	    /* Set low byte of divisor */
    outb(0x40, (divisor >> 8) & 0xFF);     	    /* Set high byte of divisor */
}

void timerinit(void) {
    timersethz(TIMER_HZ);
	irqinstall(0, timerinterrupt);
}

void timer_wait_until(uint32_t time) {
    while (_timer_ticks < time) {
        // wait for next interrupt, which will break out of halt
        halt();
    }
}

void timer_wait(uint32_t ticks) {
    timer_wait_until(_timer_ticks + ticks);
}
