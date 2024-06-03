#ifndef _STAGE2_MEMORY
#define _STAGE2_MEMORY

#include "types.h"

struct memregion {
    uint64_t address;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
};

void *membound(void);

#endif
