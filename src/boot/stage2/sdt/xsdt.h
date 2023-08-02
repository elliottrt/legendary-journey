#ifndef _STAGE2_XSDT
#define _STAGE2_XSDT

#include "../std.h"
#include "sdt.h"

/* https://wiki.osdev.org/XSDT */

struct xsdt
{
	struct acpi_sdt_header header;
	uint64_t first_entry;
};

/* initializes the RSDT */
void xsdt_init(struct xsdt *xsdt);

/* fetches the entry at index from rsdt */
void *xsdt_entry(uint32_t index);

/* fetches the entry with signature signature */
void *xsdt_find(char *signature);

#endif
