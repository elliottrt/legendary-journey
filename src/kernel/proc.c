#include "proc.h"
#include "printf.h"

static struct tss tss_entry = {0};

void proc_gdt_segment(struct gdtentry *entry) {
	uint base = (uint) &tss_entry;
	uint limit = sizeof(struct tss);

	entry->limit_lo = limit & 0xFFFF;
	entry->base_lo = base & 0xFFFF;
	entry->base_mid = (base >> 16) & 0xFF;
	entry->access = 0b10001001;
	entry->granularity = (0b0000 << 4) | ((limit >> 16) & 0xF);
	entry->base_hi = (base >> 24) & 0xFF;
}
