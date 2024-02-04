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

	struct file text;

	char contents[] = "Created folder/text.txt successfully!";

	if (fileopen(&text, "/folder/folder/../text.txt", FCREATE) < 0)
		printf("failed to open file: %d\n", errno);

	if (filewrite(&text, contents, sizeof(contents) - 1) < 0)
		printf("failed to write to file: %d\n", errno);

	if (fileclose(&text) < 0)
		printf("failed to close file: %d\n", errno);

	while(1);
}
