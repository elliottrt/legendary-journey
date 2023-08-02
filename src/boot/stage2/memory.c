#include "memory.h"
#include "write.h"

extern uint16_t _entry_memcount;
extern struct memregion _entry_memregions;

int _memregion_ignore(struct memregion *region)
{
	return (region->acpi & 1) == 0;
}

void _put_memregion(uint16_t idx)
{

	struct memregion *region = (&_entry_memregions) + idx;

	if (_memregion_ignore(region))
	{
		puts("Ignored region\n");
		return;
	}

	puth64(region->begin);
	putc(' ');
	puth64(region->len);
	putc(' ');
	putu(region->type);
	putc('\n');
}

void mem_init(void)
{
	puts("Memory Regions: ");
	putu(_entry_memcount);
	putc('\n');

	_put_memregion(0);
	_put_memregion(1);
	_put_memregion(2);
	_put_memregion(3);
	_put_memregion(4);
	_put_memregion(5);
}

int memsafe(void *ptr, uint32_t sz)
{
	struct memregion *region = &_entry_memregions;
	uint64_t region_end;
	uint16_t idx;

	for (idx = 0; idx < _entry_memcount; idx++, region++)
	{
		region_end = region->begin + region->len; /* start of next region */
		if ( (uint32_t) ptr >= region->begin && (uint32_t) ptr < region_end) /* if ptr in region */
			return region->type == FREE_MEM && (uint32_t) ptr + sz <= region_end; /* return whether it's entirely in the region */
	}
	return 0;
}
