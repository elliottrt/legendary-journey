#include "write.h"

#define _SCR_WIDTH 80
#define _SCR_HEIGHT 25
#define _SCR_MEM ((uint16_t *)0xB8000)

static uint8_t _cur_col = 0;
static uint8_t _cur_row = 0;

/* white with black background */
static uint16_t _color = 15 << 8;

void _check_cur(void)
{
    if (_cur_col >= _SCR_WIDTH)
    {
        _cur_col = 0;
        _cur_row++;
    }
    if (_cur_row >= _SCR_HEIGHT)
    {
        _cur_row = 0;
    }
}

/* put chr at the next open position */
void putc(char chr)
{
    if (chr == '\n')
    {
        _cur_col = 0;
        _cur_row++;
    }
    else
    {
        _SCR_MEM[_cur_row * _SCR_WIDTH + _cur_col] = chr | _color;
        _cur_col++;
    }
    _check_cur();
}

/* while the next character isn't \0, putc(it) */
void puts(const char *s)
{
    char c = *s++;
    for (; c != 0; c=*s++)
    {
        putc(c);
    }
}

void putsn(const char *s, uint32_t len)
{
	uint32_t current_char;
	for (current_char = 0; current_char < len; current_char++)
		putc(s[current_char]);
}

void puterr(const char *s, int recoverable)
{
	/* black on blue */
	_color = (15 | (1 << 4)) << 8;
	/* print error message */
	puts(s);
	/* hang if not recoverable */
	if (!recoverable) while(1);
	/* reset color */
	_color = 15 << 8;
}

void putu(uint32_t u)
{
    uint32_t digit = u % 10;
	uint32_t next = u / 10;
    if (next != 0)
        putu(next);
    putc(digit + '0');
}

void _puti_inner(int32_t i)
{
	uint32_t digit = i % 10;
	uint32_t next = i / 10;
    if (next != 0)
        _puti_inner(next);
    putc(digit + '0');
}

void puti(int32_t i)
{
	if (i < 0)
	{
		putc('-');
		i = -i;
	}
	_puti_inner(i);
}

char _hex_table[16] = "0123456789ABCDEF";

void puth(uint32_t h)
{
	uint8_t nibble;
	uint8_t i;

	puts("0x");

	for (i = 1; i <= 8; i++)
	{
		nibble = (h >> (32 - i * 4)) & 0x0F;
		putc(_hex_table[nibble]);
	}

}

void puth64(uint64_t h)
{
	uint8_t nibble;
	uint8_t i;

	puts("0x");

	for (i = 1; i <= 16; i++)
	{
		nibble = (h >> (64 - i * 4)) & 0x0F;
		putc(_hex_table[nibble]);
	}
}

void cursor(uint8_t row, uint8_t col)
{
    _cur_row = row;
    _cur_col = col;
    if (_cur_col >= _SCR_WIDTH)
        _cur_col = 0;
    if (_cur_row >= _SCR_HEIGHT)
        _cur_row = 0;
}

void clrscr(void)
{
    uint16_t clear_char = ' ' | _color;
    uint16_t position = 0;

    for (; position < _SCR_WIDTH * _SCR_HEIGHT; position++)
    {
        _SCR_MEM[position] = clear_char;
    }

    cursor(0, 0);
}
