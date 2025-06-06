#include "boot/stage2/write.h"
#include "common/x86.h"

#define SCR_WIDTH 80
#define SCR_HEIGHT 25
#define SCR_MEM ((uint16_t *)0xB8000)

#define EMPTY_CHAR '\0'

static uint8_t _curcol = 0;
static uint8_t _currow = 0;

/* white with black background */
static uint16_t _color = 0x0F << 8;

void _checkcur(void)
{
    if (_curcol >= SCR_WIDTH)
    {
        _curcol = 0;
        _currow++;
    }
    if (_currow >= SCR_HEIGHT)
    {
        _currow = 0;
    }
}

/* put chr at the next open position */
void putc(char chr)
{
    if (chr == '\n')
    {
        _curcol = 0;
        _currow++;
    }
    else
    {
        SCR_MEM[_currow * SCR_WIDTH + _curcol] = chr | _color;
        _curcol++;
    }
    _checkcur();
}

/* while the next character isn't \0, putc(it) */
void puts(const char *s)
{
    for (; *s; s++)
        putc(*s);
}

void puterr(const char *s, int recoverable)
{
	/* black on blue */
	_color = 0x1F << 8;
	/* print error message */
	puts(s);
	/* hang if not recoverable */
	if (!recoverable) STOP();
	/* reset color */
	_color = 0x0F << 8;
}

void clrscr(void)
{
    uint16_t clearchar = EMPTY_CHAR | _color;

    for (int position = 0; position < SCR_WIDTH * SCR_HEIGHT; position++)
        SCR_MEM[position] = clearchar;

    _curcol = 0;
	_currow = 0;
}
