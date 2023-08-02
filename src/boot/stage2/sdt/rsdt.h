#ifndef _STAGE2_RSDT
#define _STAGE2_RSDT

#include "../std.h"
#include "sdt.h"

/* https://wiki.osdev.org/RSDT */

struct rsdt
{
	struct acpi_sdt_header header;
	uint32_t first_entry;
};

/* initializes the RSDT */
void rsdt_init(struct rsdt *rsdt);

/* fetches the entry at index from rsdt */
void *rsdt_entry(uint32_t index);

/* fetches the entry with signature signature */
void *rsdt_find(char *signature);

#endif
