
#include <stdint.h>

#define CHARMEM ((uint16_t*)0xB8000)

#define CWIDTH 80
#define CHEIGHT 25

#define CS_BLACK          0
#define CS_BLUE           1
#define CS_GREEN          2
#define CS_CYAN           3
#define CS_RED            4
#define CS_MAGENTA        5
#define CS_BROWN          6
#define CS_LIGHT_GREY     7
#define CS_DARK_GREY      8
#define CS_LIGHT_BLUE     9
#define CS_LIGHT_GREEN   10
#define CS_LIGHT_CYAN    11
#define CS_LIGHT_RED     12
#define CS_LIGHT_MAGENTA 13
#define CS_LIGHT_BROWN   14
#define CS_WHITE         15

static uint8_t tm_color(uint8_t fg, uint8_t bg) 
{
    return fg | bg << 4;
}

static uint16_t tm_entry(char c, uint8_t color) 
{
    return (uint16_t) c | (uint16_t) color << 8;
}

void putc(char c, uint8_t x, uint8_t y) 
{
    CHARMEM[y * CWIDTH + x] = tm_entry(c, CS_WHITE);
}

void writez(char *data, uint8_t x, uint8_t y)
{
    char c = *data;
    for (c; c != 0; c = *(++data))
    {
        putc(c, x++, y);
    }
}

void stage2main(uint16_t boot_drive)
{
    writez("STAGE 2 BEGIN", 0, 0);
}
