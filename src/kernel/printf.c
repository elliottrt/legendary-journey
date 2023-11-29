#include "printf.h"
#include "types.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

ushort *screen_addr = (ushort *) 0xB8000;
uint cursor = 0;
uchar color = 15;

void putc(char c) {
	switch (c) {
		case '\n':
			cursor += SCREEN_WIDTH;
			cursor -= cursor % SCREEN_WIDTH;
			break;
		default:
			screen_addr[cursor++] = c | (color << 8);
			break;
	}
}

void printint(int x, int base, int sign) {
	const char digits[] = "0123456789ABCDEF";
	char buffer[16];
	int negative = 0;

	if (sign && x < 0) {
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

#define VAR_NEXT(T) (*((T*)varargs++))

void printf(const char *format, ...) {

	uint *varargs = (uint *) ((void *) &format) + 1;

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
					int x = VAR_NEXT(int);
					printint(x, 10, 1);
				} break;
				case 'u': {
					uint x = VAR_NEXT(uint);
					printint(x, 10, 0);
				} break;
				case 'p':
				case 'x': {
					uint x = VAR_NEXT(uint);
					printint(x, 16, 0);
				} break;
				case 'c': {
					char c = VAR_NEXT(char);
					putc(c);
				} break;
				case 's': {
					char *s = VAR_NEXT(char *);
					while (*s) putc(*s++);
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
