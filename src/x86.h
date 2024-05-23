#ifndef _ASMCALL
#define _ASMCALL

#include "types.h"

#define ASMCALL __attribute__((cdecl))

#ifndef asm
#define asm __asm__ volatile
#endif

#define cli() asm("cli");
#define sti() asm("sti");

#define UNUSED_PORT 0x80
#define iowait() outb(UNUSED_PORT, 0x00)

void ASMCALL outb(ushort port, uchar value);
uchar ASMCALL inb(ushort port);

void ASMCALL outw(ushort port, ushort value);
ushort ASMCALL inw(ushort port);

// for kernel::kgdt.c

void ASMCALL kgdtload(void);

// for kernel::virtmem.c

void ASMCALL lcr3(uint val);

// for kernel::idt.c

void ASMCALL idtload(void *idtptr);

#endif
