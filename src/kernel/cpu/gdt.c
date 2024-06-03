#include "gdt.h"
#include "x86.h"
#include "proc.h"

struct gdtentry gdt[GDT_SIZE] = {0};
struct gdtdesc gdtdesc = {
	.size = (sizeof(struct gdtentry) * GDT_SIZE) - 1,
	.offset = (uint) gdt,
};

void gdtinit(void) {

	gdt[GDT_NULL] = (struct gdtentry) {0}; // null descriptor
	gdt[GDT_CODE] = (struct gdtentry) { // code segment
		.limit_lo = 0xFFFF,
		.base_lo = 0x0000,
		.base_mid = 0x00,
		.access = 0x9A, // 0b10011010
		.granularity = 0xCF, // 0b11001111
		.base_hi = 0x00
	};
	gdt[GDT_DATA] = (struct gdtentry) { // data segment
		.limit_lo = 0xFFFF,
		.base_lo = 0x0000,
		.base_mid = 0x00,
		.access = 0x92, // 0b10010010
		.granularity = 0xCF, // 0b11001111
		.base_hi = 0x00
	};
	gdt[GDT_USER_CODE] = (struct gdtentry) { // user code segment
		.limit_lo = 0xFFFF,
		.base_lo = 0x0000,
		.base_mid = 0x00,
		.access = 0xFA, // 0b11111010
		.granularity = 0xCF, // 0b11001111
		.base_hi = 0x00
	};
	gdt[GDT_USER_DATA] = (struct gdtentry) { // user data segment
		.limit_lo = 0xFFFF,
		.base_lo = 0x0000,
		.base_mid = 0x00,
		.access = 0xF2, // 0b11110010
		.granularity = 0xCF, // 0b11001111
		.base_hi = 0x00
	};
	proc_gdt_segment(&gdt[GDT_TSS]); // tss segment

	gdtload();
}
