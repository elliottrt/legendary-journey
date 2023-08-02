#include "rsdt.h"
#include "../write.h"

struct rsdt *_rsdt = NULL;
uint32_t _rsdt_entry_count = 0;

void rsdt_init(struct rsdt *rsdt)
{
	if (sdt_check_table(&rsdt->header) == 0)
	{
		puts("Invalid RSDT\n");
		return;
	}

	puts("RSDP & RSDT Found\n");

	_rsdt = rsdt;
	_rsdt_entry_count = (rsdt->header.length - sizeof(struct rsdt)) / 4;
}

void *rsdt_entry(uint32_t index)
{
	if (index >= _rsdt_entry_count)
		return NULL;
	return (void *) (&_rsdt->first_entry + index);
}

void *rsdt_find(char *signature)
{
	uint32_t i = _rsdt_entry_count;
	struct acpi_sdt_header *entry;

	for (; i--;)
	{
		entry = rsdt_entry(i);
		if (strncmp(signature, entry->signature, 4))
			return entry;
	}

	return NULL;
}
