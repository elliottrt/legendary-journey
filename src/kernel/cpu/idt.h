#ifndef _KERNEL_IDT
#define _KERNEL_IDT

#include "common/types.h"

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
    uint16_t offset_lo;
    uint16_t selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t offset_hi;
} __attribute__ ((packed));

struct idtptr {
    uint16_t limit;
    uint32_t base;
} __attribute__ ((packed));

void idtset(uint8_t index, void (*base)(struct registers), uint16_t selector, enum idt_flags flags);
void idtenable(uint8_t index);
void idtdisable(uint8_t index);
void idtinit();

#endif
