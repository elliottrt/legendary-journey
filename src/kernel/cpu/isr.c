#include "isr.h"
#include "graphics/printf.h"
#include "idt.h"
#include "isrstubs.h"

static const char *exceptions[32] = {
    "division by zero",
    "debug",
    "nmi",
    "breakpoint",
    "overflow",
    "out of bounds",
    "invalid opcode",
    "no coprocessor",
    "double fault",
    "coprocessor segment overrun",
    "bad tss",
    "segment not present",
    "stack fault",
    "general protection fault",
    "page fault",
    "unrecognized interrupt",
    "coprocessor fault",
    "alignment check",
    "machine check",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved"
};

static isrhandler_t handlers[NUM_ISRS] = { 0 };

void isrinstall(uint8_t i, isrhandler_t handler) {
    handlers[i] = handler;
}

// referenced from interrupts.S
void isrhandler(struct registers *regs) {
    if (handlers[regs->int_no]) {
        handlers[regs->int_no](regs);
    } else {
        printf("unhandled interrupt %i\n", regs->int_no);
    }
}

static void exceptionhandler(struct registers *regs) {
    printf("exception: %s\n", exceptions[regs->int_no]);
    // TODO: should we try to recover?
    // TODO: page fault data -> print cr2 (which has accessed address that caused fault)
    while(1);
}

void isrinit() {
    isrfillstubs();

    for (uint32_t i = 0; i < 32; i++) {
        isrinstall(i, exceptionhandler);
        idtenable(i);
    }
}
