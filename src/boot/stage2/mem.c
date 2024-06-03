#include "mem.h"
#include "write.h"

extern uint16_t _entrymemcount;
extern struct memregion _entrymemregions[];

/* get the region of memory the kernel and all processes will use */
void *membound(void) {

    for (uint32_t i = 0; i < _entrymemcount; i++) {
        struct memregion *region = &_entrymemregions[i];

        if (region->address == KERNEL_LOAD && region->type == 1) {
            return (void *) KERNEL_LOAD + region->length;
        }

    }

    puterr("unable to find usable memory starting at KERNEL_LOAD\n", 0);
    return 0;
}
