#include "kernel/graphics/printf.h"
#include "common/types.h"
#include "common/std.h"
#include "common/mmu.h"

// TODO: try to get these from video mode?
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define EMPTY_CHAR '\0'

// should be defined in Makefile
#ifndef TAB_WIDTH
#define TAB_WIDTH 4
#endif

uint16_t *screen_addr = (uint16_t *) P2V_WO(0xB8000);
uint32_t cursor = 0;
uint8_t color = COLOR(CLR_WHITE, CLR_BLACK);

#define CURSOR_COLOR CLR_CYAN

void scroll(void) {
	uint16_t *line = screen_addr;

	for (uint32_t row = 0; row < SCREEN_HEIGHT - 1; row++) {
		memcpy(
			line + row * SCREEN_WIDTH,
			line + (row + 1) * SCREEN_WIDTH,
			SCREEN_WIDTH * sizeof(uint16_t)
		);
	}

	line = screen_addr + (SCREEN_HEIGHT - 1) * SCREEN_WIDTH;
	for (uint32_t col = 0; col < SCREEN_WIDTH; col++)
		line[col] = EMPTY_CHAR | (color << 8);
}

void move_cursor(int32_t delta) {
	char at_cursor = screen_addr[cursor] & 0xFF;
	screen_addr[cursor] = at_cursor | color << 8;

	cursor += delta;

	at_cursor = screen_addr[cursor] & 0xFF;
	screen_addr[cursor] = at_cursor | COLOR(0, CURSOR_COLOR) << 8;
}

int putchar(char c) {
	switch (c) {
		case '\n':
			move_cursor(SCREEN_WIDTH);
			move_cursor(-(cursor % SCREEN_WIDTH));
			break;
		case '\b':
			// if we need to go up a line
			if (cursor % SCREEN_WIDTH == 0) {
				// don't do anything if we are at the top left
				if (cursor == 0) return 0;

				uint32_t prev_line_start = cursor - SCREEN_WIDTH;
				while (cursor > prev_line_start && (screen_addr[cursor - 1] & 0xFF) == EMPTY_CHAR)
					move_cursor(-1);

				// fix the overshoot
				cursor++;
			}

			screen_addr[cursor - 1] = EMPTY_CHAR | (color << 8);
			move_cursor(-1);
			break;
		case '\t':
			move_cursor(TAB_WIDTH);
			move_cursor(-(cursor % TAB_WIDTH));
			break;
		case '\r':
			move_cursor(-(cursor % SCREEN_WIDTH));
			break;
		default:
			screen_addr[cursor] = c | (color << 8);
			move_cursor(1);
			break;
	}

	// when we reach end of screen, scroll
	if (cursor >= SCREEN_HEIGHT * SCREEN_WIDTH) {
		scroll();
		move_cursor(SCREEN_WIDTH * (SCREEN_HEIGHT - 1) - cursor);
	}

	return 0;
}

int puts(const char *str) {
	while (*str) putchar(*(str++));
	putchar('\n');
	return 0;
}

int putsn(const char *str, uint32_t n) {
	for (uint32_t i = 0; i < n; i++)
		putchar(str[i]);
	
	return 0;
}

// some number significantly above log10(2^32)
#define PRINTINT_BUFFER_SIZE 32
void printint(uint32_t x, int base, int sign, int padding) {
	const char digits[] = "0123456789ABCDEF";
	char buffer[PRINTINT_BUFFER_SIZE];
	int negative = 0;

	if (sign && (* (int32_t *) &x) < 0) {
		negative = 1;
		x = -x;
	}

	int i = 0;
	do {
		buffer[i++] = digits[x % base];
	} while ((x /= base) != 0);

	if (negative) buffer[i++] = '-';

	padding = min(padding, PRINTINT_BUFFER_SIZE);
	while (padding > i) {
		buffer[i++] = base == 16 ? '0' : ' ';
	}

	while(i --> 0)
		putchar(buffer[i]);
}

int printf(const char *format, ...) {
	int padding;
	bool islong;

	uint32_t *varargs = (uint32_t *) ((void *) &format) + 1;

	for (int i = 0; format[i]; i++) {
		if (format[i] != '%')
			putchar(format[i]);
		else {
			char next = format[++i];

			// calculate how much padding
			padding = 0;
			while (isdigit(next)) {
				padding = padding * 10 + (next - '0');
				next = format[++i];
			}

			// TODO: padding only affects integers. it should also affect %c and %s

			islong = next == 'l';
			if (islong)
				next = format[++i];

			switch (next) {
				case '%': {
					putchar('%');
				} break;
				case 'd': {
					printint(*varargs, 10, 1, padding);
					varargs++;
				} break;
				case 'u': {
					printint(*varargs, 10, 0, padding);
					varargs++;
				} break;
				case 'p':
				case 'x': {
					printint(*varargs, 16, 0, padding);
					varargs++;
					if (islong) {
						printint(*varargs, 16, 0, 0);
						varargs++;
					}
				} break;
				case 'c': {
					putchar(*varargs);
					varargs++;
				} break;
				case 's': {
					char *s = (char *) *varargs;
					while (*s) putchar(*s++);
					varargs++;
				} break;
				default: {
					putchar('%');
					putchar(next);
				} break;
			}
		}
	}

	return 0;
}

void printfcolor(uint8_t _color) {
	color = _color;
}
