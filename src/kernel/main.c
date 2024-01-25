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

extern void kgdtinit(void);

// must be higher than 18
#define TIMER_HZ 50

void main(void)
{

	// PHYSTOP = 0xE000000;

	printf("loaded %d bytes of kernel!\n", V2P(end) - KERNEL_LOAD);
	printf("memory bound: %d mb\n", ((PHYSTOP / 1024) + 1023)/1024);

	kallocinit();

	kpginit();

	printf("paging finished\n");

	kgdtinit();

	printf("gdt set\n");

	idtinit();

	printf("idt initialized\n");

	isrinit();

	printf("isr initialized\n");

	irqinit();

	printf("irq initialized\n");

//	kbdinit();
	timerinit(TIMER_HZ);

	printf("timer initialized\n");

	printf("initialization complete\n");

	while(1);
}
