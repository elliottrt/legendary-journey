#include "sdt.h"
#include "rsdp.h"

void sdt_init(void)
{
	rsdp_init();
}

int sdt_check_table(struct acpi_sdt_header *header)
{
	return (bytesum(header, header->length) & 0xFF) == 0;
}
