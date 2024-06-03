#ifndef _KERNEL_IDT
#define _KERNEL_IDT

#include "types.h"

enum idt_flags
{
    IDT_FLAG_GATE_TASK              = 0x5,
    IDT_FLAG_GATE_16BIT_INT         = 0x6,
    IDT_FLAG_GATE_16BIT_TRAP        = 0x7,
    IDT_FLAG_GATE_32BIT_INT         = 0xE,
    IDT_FLAG_GATE_32BIT_TRAP        = 0xF,

    IDT_FLAG_RING0                  = (0 << 5),
    IDT_FLAG_RING1                  = (1 << 5),
    IDT_FLAG_RING2                  = (2 << 5),
    IDT_FLAG_RING3                  = (3 << 5),

    IDT_FLAG_PRESENT                = 0x80,
};

struct idtentry {
    ushort offset_lo;
    ushort selector;
    uchar reserved;
    uchar flags;
    ushort offset_hi;
} __attribute__ ((packed));

struct idtptr {
    ushort limit;
    uint base;
} __attribute__ ((packed));

void idtset(uchar index, void (*base)(struct registers), ushort selector, enum idt_flags flags);
void idtenable(uchar index);
void idtdisable(uchar index);
void idtinit();

#endif
