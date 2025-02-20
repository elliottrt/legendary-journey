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
//	timerinit();

	// give user programs a few pages to use
	pg_map_range(kpgdir, USERBASE, 16, PTE_W);

	// TODO: allocate pages dynamically, so that each program gets only what it needs
	//		create a struct containing program information - stuff like open files, pages used

	// TODO: figure out how to tell the compiler that certain functions do exist - stub library
	
	// TODO: more granular allocation and free (need for txed) - can do this with pg_map_range
	// TODO: change file structure so it's opaque to the user
	// TODO: add more system functions - keyboard, stuff in std.h
	// TODO: split the Makefile? kernel, stage 1, and stage 2 could all have their own

	// TODO: write user programs like cat, mkdir, etc.
	// TODO: port txed to this

	while (1) {
		shell();
	}

	printf("initialization complete, halting.\n");
	cli(); halt();
}
