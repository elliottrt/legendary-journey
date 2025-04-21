#include "common/types.h"
#include "common/mmu.h"
#include "common/ata.h"
#include "common/fat.h"
#include "common/file.h"
#include "common/x86.h"
#include "common/elf.h"
#include "common/std.h"
#include "kernel/graphics/printf.h"
#include "kernel/cpu/idt.h"
#include "kernel/cpu/isr.h"
#include "kernel/cpu/irq.h"
#include "kernel/cpu/gdt.h"
#include "kernel/drivers/timer.h"
#include "kernel/drivers/kbd.h"
#include "kernel/memory/virtmem.h"
#include "kernel/memory/kalloc.h"
#include "kernel/memory/general_alloc.h"
#include "kernel/user/shell.h"

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
	pg_map_range(kpgdir, USERBASE, 0x100, PTE_W);

	// TODO: allocate pages dynamically, so that each program gets only what it needs
	//		create a struct containing program information - stuff like open files, pages used

	// TODO: consider making user code relocatable with -fPIC for more address space control
	
	// TODO: change file structure so it's opaque to the user
	// TODO: add more system functions - keyboard, stuff in std.h
	// TODO: split the Makefile? kernel, stage 1, and stage 2 could all have their own

	// TODO: write user programs like mkdir, touch, etc.
	// TODO: port txed to this

	gen_debug();

	while (1) {
		shell();
	}

	printf("initialization complete, halting.\n");
	cli(); halt();
}
