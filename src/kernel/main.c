#include "types.h"
#include "printf.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "timer.h"
#include "kbd.h"
#include "virtmem.h"
#include "kalloc.h"
#include "mmu.h"
#include "ata.h"
#include "fat.h"
#include "file.h"
#include "x86.h"
#include "proc.h"
#include "gdt.h"

void main(void)
{

	printf("loaded %d bytes of kernel, VADDR=0x%x\n", V2P(end) - KERNEL_LOAD, KERNBASE);
	printf("physical memory bound: %d mb\n", ((PHYSTOP / 1024) + 1023)/1024);

	kallocinit();

	kpginit();

	gdtinit();

	atainit();
	fatinit();
	fileinit();

// TODO: do process setup between here

	idtinit();
	isrinit();
	irqinit();

//	kbdinit();
//	timerinit();

	kallocexpand();

// TODO: run processes here

	printf("initialization complete\n");

	while(1);
}
