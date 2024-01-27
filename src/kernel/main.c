#include "types.h"
#include "printf.h"
#include "ata.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "timer.h"
#include "kbd.h"
#include "virtmem.h"
#include "kalloc.h"
#include "mmu.h"
#include "fat.h"
#include "x86.h"

/*
NEXT

FILE I/O

*/

void main(void)
{

	printf("loaded %d bytes of kernel!\n", V2P(end) - KERNEL_LOAD);
	printf("memory bound: %d mb\n", ((PHYSTOP / 1024) + 1023)/1024);

	kallocinit();

	kpginit();

	kgdtinit();

	atainit();

	idtinit();
	isrinit();
	irqinit();

//	kbdinit();
	timerinit();

	kallocexpand();

	printf("initialization complete\n");

	while(1);
}
