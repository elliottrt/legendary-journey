#include "types.h"
#include "printf.h"
#include "ata.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "timer.h"
#include "kbd.h"

// end of kernel memory
extern char end[];

#define TIMER_HZ 10
#define TIMER_MS (1000 / TIMER_HZ)

void main(void)
{

	printf("Loaded %d bytes of Kernel!\n", end - 0x100000);

	atainit();
	idtinit();
	isrinit();
	irqinit();
	timerinit(TIMER_HZ);
	kbdinit();

//	asm("int $0x00");

	while(1);
}
