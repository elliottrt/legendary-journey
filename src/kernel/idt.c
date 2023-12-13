#include "idt.h"
#include "std.h"

struct idtentry entries[256];
struct idtptr pointer;

// in x86.S
extern void idtload(struct idtptr *idtptr);

void idtset(uchar index, void (*base)(struct registers), ushort selector, enum idt_flags flags) {
    entries[index] = (struct idtentry) {
        .offset_low = ((uint) base) & 0xFFFF,
        .offset_high = (((uint) base) >> 16) & 0xFFFF,
        .selector = selector,
        .flags = flags,
        ._ignored = 0
    };
}

void idtenable(uchar index) {
    entries[index].flags |= IDT_FLAG_PRESENT;
}

void idtdisable(uchar index) {
    entries[index].flags &= ~IDT_FLAG_PRESENT;
}

void idtinit() {
    pointer.limit = sizeof(entries) - 1;
    pointer.base = (uint) entries;
    memset(entries, 0x00, sizeof(entries));
    idtload(&pointer);
}
