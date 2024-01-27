#include "mem.h"
#include "write.h"

struct memregion {
    ullong address;
    ullong length;
    uint type;
    uint attributes;
};

extern ushort _entrymemcount;
extern struct memregion _entrymemregions[];

/*
static char hex[] = "0123456789abcdef";
static char *memtype[] = {
    "usable\n",
    "reserved\n",
    "acpi reclaimable\n",
    "acpi nvs\n",
    "bad\n"
};

void putx(ullong x) {
    puts("0x");
    for (int nibble = sizeof(x) * 8 - 4; nibble >= 0; nibble -= 4) {
        putc(hex[(x >> nibble) & 0x0F]);
    }
}
*/

char *membound(void) {
//    puts("memregions:\n");
    for (uint i = 0; i < _entrymemcount; i++) {
        struct memregion *region = &_entrymemregions[i];

        /*
        putx(region->address);
        putc(' ');
        putx(region->address + region->length);
        putc(' ');
        puts(memtype[region->type - 1]);
        */

        if (region->address == KERNEL_LOAD && region->type == 1) {
            return (char *) KERNEL_LOAD + region->length;
        }

    }

    puterr("unable to find usable memory starting at KERNEL_LOAD\n", 0);
    return 0;
}
