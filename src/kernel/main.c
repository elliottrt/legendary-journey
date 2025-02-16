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

	// TODO: do something after initialization

	struct file f;
	char buf[32] = {0};

	if (fileopen(&f, "/test.txt", 0)) {
		if (fileread(&f, buf, sizeof(buf) - 1)) {
			printf("read: %s\n", buf);
		} else {
			printf("failed to read file: %s\n", strerror(errno));
		}

		if (!fileclose(&f))
			printf("failed to close file: %s\n", strerror(errno));
	} else {
		printf("failed to open file: %s\n", strerror(errno));
	}

	printf("initialization complete\n");

	while(1);
}
