#include "irq.h"
#include "idt.h"
#include "isr.h"
#include "graphics/printf.h"
#include "x86.h"

// PIC constants
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
// #define PIC1_OFFSET 0x20

#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
// #define PIC2_OFFSET 0x28

#define PIC_EOI 0x20

#define PIC_REMAP_OFFSET 0x20

uchar autoeoi = 0;
ushort mask = 0xFFFF;

enum PIC_ICW1 {
    PIC_ICW1_ICW4           = 0x01,
    PIC_ICW1_SINGLE         = 0x02,
    PIC_ICW1_INTERVAL4      = 0x04,
    PIC_ICW1_LEVEL          = 0x08,
    PIC_ICW1_INITIALIZE     = 0x10
};

enum PIC_ICW4 {
    PIC_ICW4_8086           = 0x1,
    PIC_ICW4_AUTO_EOI       = 0x2,
    PIC_ICW4_BUFFER_MASTER  = 0x4,
    PIC_ICW4_BUFFER_SLAVE   = 0x0,
    PIC_ICW4_BUFFERRED      = 0x8,
    PIC_ICW4_SFNM           = 0x10,
};

enum PIC_CMD {
    PIC_CMD_END_OF_INTERRUPT    = 0x20,
    PIC_CMD_READ_IRR            = 0x0A,
    PIC_CMD_READ_ISR            = 0x0B,
};

static irqhandler_t handlers[16] = { 0 };

static void irqstub(struct registers *regs) {
    uint irq = regs->int_no - PIC_REMAP_OFFSET;

    if (handlers[irq] != NULL) {
        handlers[irq](regs);
    } else {
        printferr();
        printf("unhandled irq: %i\n", irq);
        printfstd();
    }

    // send EOI
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);

}

void irqsetmask(ushort new_mask) {
    mask = new_mask;
    outb(PIC1_DATA, mask & 0xFF);
    iowait();
    outb(PIC2_DATA, mask >> 8);
    iowait();
}

static void irqremap(uchar offset_pic1, uchar offset_pic2) {
    irqsetmask(mask);

    outb(PIC1_COMMAND, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    iowait();
    outb(PIC2_COMMAND, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    iowait();

    outb(PIC1_DATA, offset_pic1);
    iowait();
    outb(PIC2_DATA, offset_pic2);
    iowait();

    outb(PIC1_DATA, 0x4); // slave at irq2
    iowait();
    outb(PIC2_DATA, 0x2); // cascade identity
    iowait();

    uchar ecw4 = PIC_ICW4_8086;
    if (autoeoi) ecw4 |= PIC_ICW4_AUTO_EOI;

    outb(PIC1_DATA, ecw4);
    iowait();
    outb(PIC2_DATA, ecw4);
    iowait();

    irqsetmask(mask);
}

void irqenable(uint i) {
    irqsetmask(mask & ~(1 << i));
}

void irqdisable(uint i) {
    irqsetmask(mask | (1 << i));
}

void irqinstall(uint i, irqhandler_t handler) {
    cli();
    handlers[i] = handler;
    irqenable(i);
    idtenable(i + PIC_REMAP_OFFSET);
    sti();
}

void irqinit() {
    irqremap(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8);

    for (uint i = 0; i < 16; i++) {
        isrinstall(PIC_REMAP_OFFSET + i, irqstub);
    }
}