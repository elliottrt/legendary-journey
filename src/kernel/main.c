#include "common/mmu.h"
#include "common/ata.h"
#include "common/fat.h"
#include "common/file.h"
#include "common/x86.h"
#include "kernel/cpu/idt.h"
#include "kernel/cpu/isr.h"
#include "kernel/cpu/irq.h"
#include "kernel/cpu/gdt.h"
#include "kernel/drivers/timer.h"
#include "kernel/drivers/kbd.h"
#include "kernel/graphics/printf.h"
#include "kernel/memory/virtmem.h"
#include "kernel/memory/kalloc.h"
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
	timerinit();

	// give user programs a few pages to use
	pg_map_range(kpgdir, USERBASE, 0x100, PTE_W);

	// TODO: allocate pages dynamically, so that each program gets only what it needs

	// TODO: consider making user code relocatable with -fPIC for more address space control
	// TODO: shouldn't it be simple to copy the kernel page table before adding user space, then set that table while running the user program? would allow swapping between user programs and having multiple loaded in memory at once.

	// TODO: floating point support, see https://wiki.osdev.org/FPU

	// TODO: add more system functions - keyboard, stuff in std.h
	// TODO: split the Makefile? kernel, stage 1, and stage 2 could all have their own
	// TODO: use cmake? or something that allows more IDE integration

	// TODO: write user programs
	// TODO: port txed to this
	// TODO: that shell feature where if a program didn't end with a new line it'll print a weird char and go to new line

	// TODO: interrupts for kernel function calls instead of standard library dynamic linking. use some interrupt number and calling conventions to have a function like https://man7.org/linux/man-pages/man2/syscall.2.html

	do {
		//rm_debug(rm_global_ctx);
		shell();
	} while (1);

	printf("initialization complete, halting.\n");
	STOP();
}
