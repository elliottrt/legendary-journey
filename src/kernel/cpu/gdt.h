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
	uint16_t limit_lo;
	uint16_t base_lo;
	uint8_t base_mid;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_hi;
} __attribute__((packed));

struct gdtdesc {
	uint16_t size;
	uint32_t offset;
} __attribute__((packed));

void gdtinit(void);

#endif
