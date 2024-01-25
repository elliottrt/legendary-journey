#include "timer.h"
#include "printf.h"
#include "x86.h"
#include "irq.h"

static ulong ticks = 0;

void timerinterrupt(struct registers *regs) {
	ticks++;
    printf("a");
}

void timerinit(uint hz) {
    timersethz(hz);
    printf("timer set hz\n");
	irqinstall(0, timerinterrupt);
	printf("timer installed irq\n");
}

ulong timerget(void) {
	return ticks;
}

void timersethz(uint hz) {
    uint divisor = 1193180 / hz; 	/* Calculate our divisor */
    outb(0x43, 0x36);             	/* Set our command byte 0x36 */
    outb(0x40, divisor & 0xFF);   	/* Set low byte of divisor */
    outb(0x40, divisor >> 8);     	/* Set high byte of divisor */
}