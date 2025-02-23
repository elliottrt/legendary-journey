#include "kernel/cpu/idt.h"
#include "common/std.h"
#include "common/x86.h"

struct idtentry entries[256] = {0};
struct idtptr pointer = {0};

void idtset(uint8_t index, void (*base)(struct registers), uint16_t selector, enum idt_flags flags) {
    entries[index] = (struct idtentry) {
        .offset_lo = ((uint32_t) base) & 0xFFFF,
        .offset_hi = (((uint32_t) base) >> 16) & 0xFFFF,
        .selector = selector,
        .flags = flags,
        .reserved = 0
    };
}

void idtenable(uint8_t index) {
    entries[index].flags |= IDT_FLAG_PRESENT;
}

void idtdisable(uint8_t index) {
    entries[index].flags &= ~IDT_FLAG_PRESENT;
}

void idtinit() {
    pointer.limit = sizeof(entries) - 1;
    pointer.base = (uint32_t) entries;
    memset(entries, 0x00, sizeof(entries));
    idtload(&pointer);
}
