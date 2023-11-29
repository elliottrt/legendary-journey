#ifndef _STAGE2_ASMCALL
#define _STAGE2_ASMCALL

#include "types.h"

#define ASMCALL __attribute__((cdecl))

void ASMCALL outb(ushort port, uchar value);
uchar ASMCALL inb(ushort port);

void ASMCALL outw(ushort port, ushort value);
ushort ASMCALL inw(ushort port);

#endif
