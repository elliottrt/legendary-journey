#include "gdt.h"
#include "x86.h"

struct gdtentry gdt[GDT_SIZE] = {0};
struct gdtdesc gdtdesc = {
	.size = (sizeof(struct gdtentry) * GDT_SIZE) - 1,
	.offset = (uint32_t) gdt,
};

void gdtinit(void) {
	gdt[GDT_NULL] = (struct gdtentry) {0}; // null descriptor
	gdt[GDT_CODE] = (struct gdtentry) { // code segment
		.limit_low = 0xFFFF,
		.base_low = 0x000000,
		.accessed = 0,
		.read_write = 1,
		.conforming_expand_down = 0,
		.code = 1,
		.code_data_segment = 1,
		.DPL = 0,
		.present = 1,
		.limit_high = 0xF,
		.available = 0,
		.long_mode = 0,
		.big = 1,
		.gran = 1,
		.base_high = 0x00
	};
	gdt[GDT_DATA] = (struct gdtentry) { // data segment
		.limit_low = 0xFFFF,
		.base_low = 0x000000,
		.accessed = 0,
		.read_write = 1,
		.conforming_expand_down = 0,
		.code = 0,
		.code_data_segment = 1,
		.DPL = 0,
		.present = 1,
		.limit_high = 0xF,
		.available = 0,
		.long_mode = 0,
		.big = 1,
		.gran = 1,
		.base_high = 0x00
	};

	gdt[GDT_USER_CODE] = (struct gdtentry) { // user code segment
		.limit_low = 0xFFFF,
		.base_low = 0x000000,
		.accessed = 0,
		.read_write = 1,
		.conforming_expand_down = 0,
		.code = 1,
		.code_data_segment = 1,
		.DPL = 3,
		.present = 1,
		.limit_high = 0xF,
		.available = 1,
		.long_mode = 0,
		.big = 1,
		.gran = 1,
		.base_high = 0x00
	};
	gdt[GDT_USER_DATA] = (struct gdtentry) { // user data segment
		.limit_low = 0xFFFF,
		.base_low = 0x000000,
		.accessed = 0,
		.read_write = 1,
		.conforming_expand_down = 0,
		.code = 0,
		.code_data_segment = 1,
		.DPL = 3,
		.present = 1,
		.limit_high = 0xF,
		.available = 1,
		.long_mode = 0,
		.big = 1,
		.gran = 1,
		.base_high = 0x00
	};

	gdtload();
}
