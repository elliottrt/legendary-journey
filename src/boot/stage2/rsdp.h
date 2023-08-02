#ifndef _STAGE2_RSDP
#define _STAGE2_RSDP

#include "std.h"

/* https://wiki.osdev.org/RSDP */

struct rsdp_desc
{
	char signature[8];
	uint8_t checksum;
	char oemid[6];
	uint8_t revision;
	uint32_t rsdt_addr;
} __attribute__ ((packed));

struct rsdp_desc_ext
{
	char signature[8];
	uint8_t checksum;
	char oemid[6];
	uint8_t revision;
	uint32_t rsdt_addr;

	uint32_t length;
	uint64_t xsdt_addr;
	uint8_t ext_checksum;
	uint8_t reserved[3];
} __attribute__ ((packed));

/* initialize rsdp */
void rsdp_init(void);
/* find rsdp */
struct rsdp_desc_ext *rsdp_find_desc(void);

#endif
