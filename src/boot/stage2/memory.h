#ifndef _STAGE2_MEMORY
#define _STAGE2_MEMORY

#include <stdint.h>

/* See https://wiki.osdev.org/Detecting_Memory_(x86) */

/*  
	for memory allocation, we should find a place in memory
	and use a single function to request that memory, no
	freeing allowed. that way we can do what we need to and
	later overwrite all of that data with the kernel
*/

enum memregiontype
{
	FREE_MEM = 1,
	RESERVED_MEM = 2,
	ACPI_RECLAIM_MEM = 3,
	ACPI_NVS_MEM = 4,
	BAD_MEM = 5
};

struct memregion
{
	uint64_t begin, len;
	uint32_t type;
	uint32_t acpi;
};

void mem_init(void);

/* Tests whether writing or reading from ptr with a size of sz is safe. */
int memsafe(void *ptr, uint32_t sz);

#endif
