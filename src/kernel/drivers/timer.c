#include "timer.h"
#include "graphics/printf.h"
#include "x86.h"
#include "cpu/irq.h"
#include "std.h"

uint64_t _timer_ticks = 0;

void timerinterrupt(struct registers *regs) {
    UNUSED(regs);

	_timer_ticks++;
}

void timersethz(uint32_t hz) {
    uint16_t divisor = 1193180 / hz; 	/* Calculate our divisor */
    outb(0x43, 0x36);             	/* Set our command byte 0x36 */
    outb(0x40, divisor & 0xFF);   	/* Set low byte of divisor */
    outb(0x40, divisor >> 8);     	/* Set high byte of divisor */
}

void timerinit(void) {
    timersethz(TIMER_HZ);
	irqinstall(0, timerinterrupt);
}