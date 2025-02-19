#include "types.h"
#include "graphics/printf.h"
#include "cpu/idt.h"
#include "cpu/isr.h"
#include "cpu/irq.h"
#include "cpu/gdt.h"
#include "drivers/timer.h"
#include "drivers/kbd.h"
#include "memory/virtmem.h"
#include "memory/kalloc.h"
#include "mmu.h"
#include "ata.h"
#include "fat.h"
#include "file.h"
#include "x86.h"
#include "elf.h"
#include "std.h"
#include "user/shell.h"

void main(void) {

	kallocinit();

	printf("loaded %d bytes of kernel, VADDR=0x%x\n", V2P(end) - KERNEL_LOAD, KERNBASE);

	kpginit();

	gdtinit();

	atainit();
	fatinit();
	fileinit();

	idtinit();
	isrinit();
	irqinit();

	kallocexpand();

	kbdinit();
	timerinit();

	// TODO: add more system functions
	// TODO: figure out how to tell the compiler that certain functions do exist - stub library?
	// TODO: write user programs like cat, mkdir, etc.
	// TODO: port txed to this
	// TODO: /ls /folder/.. fails... why? -> note that first cluster is 0 -> implies file isn't read but no error was returned

	while (1) {
		shell();
	}

	printf("initialization complete, halting.\n");
	cli(); halt();
}
