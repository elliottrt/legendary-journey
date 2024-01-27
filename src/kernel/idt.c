#include "idt.h"
#include "std.h"
#include "x86.h"

struct idtentry entries[256];
struct idtptr pointer;

void idtset(uchar index, void (*base)(struct registers), ushort selector, enum idt_flags flags) {
    entries[index] = (struct idtentry) {
        .offset_lo = ((uint) base) & 0xFFFF,
        .offset_hi = (((uint) base) >> 16) & 0xFFFF,
        .selector = selector,
        .flags = flags,
        .reserved = 0
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
