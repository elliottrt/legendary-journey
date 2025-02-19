#ifndef _ASMCALL
#define _ASMCALL

#include "types.h"

#define ASMCALL __attribute__((cdecl))

#ifndef asm
#define asm __asm__ volatile
#endif

#define cli() asm("cli")
#define sti() asm("sti")
#define halt() asm("hlt")

#define UNUSED_PORT 0x80
#define iowait() outb(UNUSED_PORT, 0x00)

void ASMCALL outb(uint16_t port, uint8_t value);
uint8_t ASMCALL inb(uint16_t port);

void ASMCALL outw(uint16_t port, uint16_t value);
uint16_t ASMCALL inw(uint16_t port);

// for kernel::proc.c

void ASMCALL ltr(uint16_t gdtsegment);

// for kernel::gdt.c

void ASMCALL gdtload(void);

// for kernel::virtmem.c

void ASMCALL lcr3(uint32_t val);

// for kernel::idt.c

void ASMCALL idtload(void *idtptr);

#endif
