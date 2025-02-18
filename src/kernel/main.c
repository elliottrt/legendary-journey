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

//	kbdinit();
//	timerinit();

	kallocexpand();

	struct file elf;
	user_entry_t entry;

	if (fileopen(&elf, "/print", 0)) {

		if (elfread(&elf, (void *) USERBASE, &entry)) {
			// TODO: will this work with interrupts?
			printf("jumping to user code... ");
			int result = entry(0, NULL);

			if (result != 0) {
				printf("\nbad exit code: %d\n", result);
			}
		} else {
			printf("failed to read elf file: %s\n", strerror(errno));
		}
	} else {
		printf("failed to open file '/print': %s\n", strerror(errno));
		printf("this may be because you forgot to do 'make user' first\n");
	}

	printf("initialization complete, halting.\n");

	halt();
}
