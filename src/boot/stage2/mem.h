#ifndef _STAGE2_MEMORY
#define _STAGE2_MEMORY

#include "types.h"

struct memregion {
    ullong address;
    ullong length;
    uint type;
    uint attributes;
};

char *membound(void);

#endif
