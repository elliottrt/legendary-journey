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

void main(void)
{

	printf("loaded %d bytes of kernel!\n", V2P(end) - KERNEL_LOAD);
	printf("memory bound: %d mb\n", ((PHYSTOP / 1024) + 1023)/1024);

	kallocinit();

	kpginit();

	kgdtinit();

	atainit();
	fatinit();
	fileinit();

	idtinit();
	isrinit();
	irqinit();

//	kbdinit();
//	timerinit();

	kallocexpand();

	printf("initialization complete\n");

	struct file file;

	if (fileopen(&file, "/test.txt", FCREATE) < 0)
		printf("fopen: %d\n", errno);

	if (filewrite(&file, "file created successfully!", 26) < 0)
		printf("fwrite: %d\n", errno);

	if (fileclose(&file) < 0)
		printf("fclose: %d\n", errno);

	while(1);
}
