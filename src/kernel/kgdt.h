#ifndef _KERNEL_KGDT
#define _KERNEL_KGDT

#include "types.h"

#define KGDT_SIZE 6

struct gdtentry {
	ushort limit_lo;
	ushort base_lo;
	uchar base_mid;
	uchar access;
	uchar granularity;
	uchar base_hi;
} __attribute__((packed));

struct gdtdesc {
	ushort size;
	uint offset;
} __attribute__((packed));

void kgdtinit(void);

#endif
