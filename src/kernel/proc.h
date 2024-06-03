#ifndef _KERNEL_PROC
#define _KERNEL_PROC

#include "types.h"
#include "cpu/gdt.h"

// See https://wiki.osdev.org/Getting_to_Ring_3
struct tss {
	uint previous_tss;
	uint esp0;
	uint ss0;
	uint esp1;
	uint ss1;
	uint esp2;
	uint ss2;
	uint cr3;
	uint eip;
	uint eflags;
	uint eax;
	uint ecx;
	uint edx;
	uint ebx;
	uint esp;
	uint ebp;
	uint esi;
	uint edi;
	uint es;
	uint cs;
	uint ss;
	uint ds;
	uint fs;
	uint gs;
	uint ldt;
	ushort trap;
	ushort iomap_base;
} __attribute__((packed));

void proc_gdt_segment(struct gdtentry *entry);

#endif
