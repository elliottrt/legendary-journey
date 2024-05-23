#include "kgdt.h"
#include "x86.h"
#include "proc.h"

struct gdtentry kgdt[KGDT_SIZE] = {0};
struct gdtdesc kgdtdesc = {
	.size = (sizeof(struct gdtentry) * KGDT_SIZE) - 1,
	.offset = (uint) kgdt,
};

void kgdtinit(void) {

	kgdt[0] = (struct gdtentry) {0}; // null descriptor
	kgdt[1] = (struct gdtentry) { // code segment
		.limit_lo = 0xFFFF,
		.base_lo = 0x0000,
		.base_mid = 0x00,
		.access = 0b10011010,
		.granularity = 0b11001111,
		.base_hi = 0x00
	};
	kgdt[2] = (struct gdtentry) { // data segment
		.limit_lo = 0xFFFF,
		.base_lo = 0x0000,
		.base_mid = 0x00,
		.access = 0b10010010,
		.granularity = 0b11001111,
		.base_hi = 0x00
	};
	kgdt[3] = (struct gdtentry) { // user code segment
		.limit_lo = 0xFFFF,
		.base_lo = 0x0000,
		.base_mid = 0x00,
		.access = 0b11111010,
		.granularity = 0b11001111,
		.base_hi = 0x00
	};
	kgdt[4] = (struct gdtentry) { // user data segment
		.limit_lo = 0xFFFF,
		.base_lo = 0x0000,
		.base_mid = 0x00,
		.access = 0b11110010,
		.granularity = 0b11001111,
		.base_hi = 0x00
	};
	proc_gdt_segment(&kgdt[5]); // tss segment

	kgdtload();
}
