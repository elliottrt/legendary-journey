#include "xsdt.h"
#include "../write.h"

struct xsdt *_xsdt = NULL;
uint32_t _xsdt_entry_count = 0;

void xsdt_init(struct xsdt *xsdt)
{
	if (sdt_check_table(&xsdt->header) == 0)
	{
		puts("Invalid XSDT\n");
		return;
	}

	puts("RSDP & XSDT Found\n");

	_xsdt = xsdt;
	_xsdt_entry_count = (xsdt->header.length - sizeof(struct xsdt)) / 8;
}

void *xsdt_entry(uint32_t index)
{
	if (index >= _xsdt_entry_count)
		return NULL;
	return (void *) (&_xsdt->first_entry + index);
}

void *xsdt_find(char *signature)
{
	uint32_t i = _xsdt_entry_count;
	struct acpi_sdt_header *entry;

	for (; i--;)
	{
		entry = xsdt_entry(i);
		if (strncmp(signature, entry->signature, 4))
			return entry;
	}

	return NULL;
}
