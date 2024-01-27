#include "types.h"
#include "printf.h"
#include "ata.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "timer.h"
#include "kbd.h"
#include "virtmem.h"
#include "x86.h"
#include "kalloc.h"
#include "mmu.h"

// must be higher than 18
#define TIMER_HZ 100

void main(void)
{

	printf("loaded %d bytes of kernel!\n", V2P(end) - KERNEL_LOAD);
	printf("memory bound: %d mb\n", ((PHYSTOP / 1024) + 1023)/1024);

	kallocinit();

	kpginit();

	kgdtinit();

	kallocexpand();

	idtinit();
	isrinit();
	irqinit();

//	kbdinit();
	timerinit(TIMER_HZ);

	printf("initialization complete\n");

	for (uint i = 0; i < 100; i++) {
		printf("%d\n", i);
		ulong next = timerget() + 25;
		while (timerget() < next);
	}

	while(1);
}
