#ifndef _STAGE2_SDT
#define _STAGE2_SDT

#include "../std.h"

struct acpi_sdt_header
{
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oemid[6];
	char oemtableid[8];
	uint32_t oemrevision;
	uint32_t creatorid;
	uint32_t creatorrevision;
} __attribute__ ((packed));

void sdt_init(void);

int sdt_check_table(struct acpi_sdt_header *header);

#endif
