#ifndef _KERNEL_GDT
#define _KERNEL_GDT

#include "types.h"

#define GDT_NULL 		0
#define GDT_CODE 		1
#define GDT_DATA 		2
#define GDT_USER_CODE 	3
#define GDT_USER_DATA 	4
#define GDT_TSS 		5
#define GDT_SIZE 		6

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

void gdtinit(void);

#endif
