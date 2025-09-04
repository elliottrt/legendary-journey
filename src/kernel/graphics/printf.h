#ifndef _KERNEL_PRINTF
#define _KERNEL_PRINTF

#include "common/types.h"

enum color {
	CLR_BLACK = 0,
	CLR_BLUE = 1,
	CLR_GREEN = 2,
	CLR_CYAN = 3,
	CLR_RED = 4,
	CLR_MAGENTA = 5,
	CLR_BROWN = 6,
	CLR_LIGHT_GRAY = 7,
	CLR_DARK_GRAY  = 8,
	CLR_LIGHT_BLUE = 9,
	CLR_LIGHT_GREEN = 10,
	CLR_LIGHT_CYAN = 11,
	CLR_LIGHT_RED = 12,
	CLR_LIGHT_MAGENTA = 13,
	CLR_LIGHT_BROWN = 14,
	CLR_WHITE = 15
};

#define COLOR(FG, BG) ((FG) | (BG) << 4)

int putc(char ch);
int puts(const char *str);
int putsn(const char *str, uint32_t n);

int printf(const char *format, ...);

void printfcolor(uint8_t color);

#endif
