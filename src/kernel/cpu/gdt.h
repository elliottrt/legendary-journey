#ifndef _KERNEL_GDT
#define _KERNEL_GDT

#include "common/types.h"

#define GDT_NULL 		0
#define GDT_CODE 		1
#define GDT_DATA 		2
#define GDT_USER_CODE 	3
#define GDT_USER_DATA 	4
#define GDT_TSS 		5
#define GDT_SIZE 		6

// copied from https://wiki.osdev.org/Getting_to_Ring_3
struct gdtentry {
	unsigned int limit_low 				: 16;
	unsigned int base_low 				: 24;
	unsigned int accessed               :  1;
	unsigned int read_write             :  1;
	unsigned int conforming_expand_down :  1;
	unsigned int code                   :  1;
	unsigned int code_data_segment      :  1;
	unsigned int DPL                    :  2;
	unsigned int present                :  1;
	unsigned int limit_high             :  4;
	unsigned int available              :  1;
	unsigned int long_mode              :  1;
	unsigned int big                    :  1;
	unsigned int gran                   :  1;
	unsigned int base_high              :  8;
} __attribute__((packed));

struct gdtdesc {
	uint16_t size;
	uint32_t offset;
} __attribute__((packed));

void gdtinit(void);

#endif
