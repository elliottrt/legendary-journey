#ifndef _STAGE2_WRITE
#define _STAGE2_WRITE

/* put character 'c' at next position and update position */
void putc(char c);
/* put null terminated string 's' at next position and update position */
void puts(const char *s);
/* print an error message 's' */
void puterr(const char *s, int recoverable);
/* clear the screen */
void clrscr(void);

#endif
