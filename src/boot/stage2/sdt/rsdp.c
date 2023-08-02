#include "rsdp.h"
#include "../write.h"
#include "rsdt.h"
#include "xsdt.h"

#define EBDAPTR ((uint16_t *) 0x040E)
#define EBDASIZ (1024/8)
#define MBASTART 0x000E0000
#define MBAEND 0x000FFFFF

void rsdp_init(void)
{
	uint32_t checksum;
	struct rsdp_desc_ext *rsdp = rsdp_find_desc();

	if (rsdp == NULL)
	{
		puts("Unable to find rsdp\n");
		return;
	}

	if (rsdp->revision == 0)
	{
		checksum = bytesum(rsdp, sizeof(struct rsdp_desc));
		if ((checksum & 0xFF) != 0)
			puts("RSDP Invalid\n");
		else
			rsdt_init((void *) rsdp->rsdt_addr);
	}
	else
	{
		checksum = bytesum(rsdp, sizeof(struct rsdp_desc_ext));
		if ((checksum & 0xFF) != 0)
			puts("RSDP Invalid\n");
		else /* this is really bad but i'll fix it later */
			xsdt_init((void *) (uint32_t) rsdp->xsdt_addr);
	}
}

struct rsdp_desc_ext *rsdp_find_desc(void)
{
	/* find the start of the EBDA */
	char *ebda = (char *) (uint32_t) *EBDAPTR;
	uint32_t i;

	for (i = 0; i < EBDASIZ; i++, ebda += 8)
	{
		if (strncmp(ebda, "RSD PTR ", 8) == 0)
			return (struct rsdp_desc_ext *) ebda;
	}

	ebda = (char *) MBASTART;
	for (i = 0; i < MBAEND - MBASTART; i += 8, ebda += 8)
	{
		if (strncmp(ebda, "RSD PTR ", 8) == 0)
			return (struct rsdp_desc_ext *) ebda;
	}

	return NULL;
}
