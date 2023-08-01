#ifndef _STAGE2_ASMCALL
#define _STAGE2_ASMCALL

#include <stdint.h>

#define ASMCALL __attribute__((cdecl))

void ASMCALL outb(uint16_t port, uint8_t value);
uint8_t ASMCALL inb(uint16_t port);

void ASMCALL outw(uint16_t port, uint16_t value);
uint16_t ASMCALL inw(uint16_t port);

#endif
