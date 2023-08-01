#ifndef _STAGE2_WRITE
#define _STAGE2_WRITE

#include <stdint.h>

/* put character 'c' at next position and update position */
void putc(char c);
/* put null terminated string 's' at next position and update position */
void puts(const char *s);
/* put string 's' of length 'len' at next position and update position */
void putsn(const char *s, uint32_t len);
/* print an error message 's' */
void puterr(const char *s, int recoverable);
/* put unsigned 32 bit integer 'u' in decimal at next position and update position */
void putu(uint32_t u);
/* put signed 32 bit integer 'i' in decimal at next position and update position */
void puti(int32_t i);
/* put 32 bit integer 'h' in hex at next position and update position */
void puth(uint32_t h);
/* move cursor to specific position */
void cursor(uint8_t row, uint8_t col);
/* clear the screen */
void clrscr(void);

#endif
