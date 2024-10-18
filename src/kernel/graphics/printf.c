#include "printf.h"
#include "types.h"
#include "std.h"
#include "memory/mmu.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define EMPTY_CHAR '\0'

uint16_t *screen_addr = (uint16_t *) P2V_WO(0xB8000);
uint32_t cursor = 0;
uint8_t color = 15;

void scroll(void) {

	uint16_t *line = screen_addr;

	for (uint32_t row = 0; row < SCREEN_HEIGHT - 1; row++, line += SCREEN_WIDTH) {
		memcpy(line, line + SCREEN_WIDTH, SCREEN_WIDTH * sizeof(uint16_t));
	}

	for (uint32_t col = 0; col < SCREEN_WIDTH; col++)
		line[col] = EMPTY_CHAR | (color << 8);
}

void putc(char c) {
	switch (c) {
		case '\n':
			cursor += SCREEN_WIDTH;
			cursor -= cursor % SCREEN_WIDTH;
			break;
		case '\b':
			// we need to go up a line
			if (cursor % SCREEN_WIDTH == 0) {
				// don't do anything if we are at the start
				if (cursor == 0) return;

				while ((screen_addr[--cursor] & 0xFF) == EMPTY_CHAR);
				// fix the overshoot
				cursor++;
			}
			screen_addr[--cursor] = EMPTY_CHAR | (color << 8);
			break;
		default:
			screen_addr[cursor++] = c | (color << 8);
			break;
	}

	// when we reach end of screen, scroll
	if (cursor >= SCREEN_HEIGHT * SCREEN_WIDTH) {
		scroll();
		cursor = SCREEN_WIDTH * (SCREEN_HEIGHT - 1);
	}

}

void printint(uint32_t x, int base, int sign) {
	
	const char digits[] = "0123456789ABCDEF";
	char buffer[16];
	int negative = 0;

	if (sign && (* (int *) &x) < 0) {
		negative = 1;
		x = -x;
	}

	int i = 0;
	do {
		buffer[i++] = digits[x % base];
	} while ((x /= base) != 0);

	if (negative) buffer[i++] = '-';

	while(--i >= 0)
    	putc(buffer[i]);

}

// TODO: support for 64 bit ints and padding
// ex: %ld and %<x>d
void printf(const char *format, ...) {

	uint32_t *varargs = (uint32_t *) ((void *) &format) + 1;

	for (int i = 0; format[i]; i++) {
		if (format[i] != '%')
			putc(format[i]);
		else {
			char next = format[++i];
			switch (next) {
				case '%': {
					putc('%');
				} break;
				case 'd': {
					printint(*varargs, 10, 1);
					varargs++;
				} break;
				case 'u': {
					printint(*(uint32_t *)varargs, 10, 0);
					varargs++;
				} break;
				case 'p':
				case 'x': {
					printint(*(uint32_t *)varargs, 16, 0);
					varargs++;
				} break;
				case 'c': {
					putc(*varargs);
					varargs++;
				} break;
				case 's': {
					char *s = (char *) *varargs;
					while (*s) putc(*s++);
					varargs++;
				} break;
				default: {
					putc('%');
					putc(next);
				} break;
			}
		}
	}

}

void printfcolor(enum color fg, enum color bg) {
	color = fg | bg << 4;
}
