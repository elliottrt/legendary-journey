#include "proc.h"
#include "graphics/printf.h"
#include "x86.h"

static struct tss tss_entry = {0};

void proc_gdt_segment(struct gdtentry *entry) {
	uint32_t base = (uint32_t) &tss_entry;
	uint32_t limit = sizeof(struct tss);

	entry->limit_lo = limit & 0xFFFF;
	entry->base_lo = base & 0xFFFF;
	entry->base_mid = (base >> 16) & 0xFF;
	entry->access = 0x89; // 0b10001001
	entry->granularity = (0 /* 0b0000 */ << 4) | ((limit >> 16) & 0xF);
	entry->base_hi = (base >> 24) & 0xFF;

	printf("you have stuff to do in %s, line %d\n", __FILE__, __LINE__); while(1);

	// TODO: need to set this, hangs otherwise
	tss_entry.esp0 = 0;
	tss_entry.ss0 = GDT_DATA << 3;

	ltr(GDT_TSS << 3);
}
