#ifndef _STAGE2_STD
#define _STAGE2_STD

#include "types.h"

#define NULL 0

/* See https://www.ibm.com/docs/en/i/7.3?topic=extensions-standard-c-library-functions-table-by-name */
/* descriptions copied from there */

/* Tests if c is a lowercase letter. */
int islower(char c);

/* Compares up to n bytes of buf1 and buf2. */
int memcmp(const void *buf1, const void *buf2, uint n);

/* Copies n bytes of src to dest. */
void *memcpy(void *dst, const void *src, uint n);

/* Sets n bytes of dst to a value c. */
void *memset(void *dst, uchar c, uint n);

/* Locates the first occurrence of c in str. */
char *strchr(const char *str, char c);

/* Calculates the length of str. */
uint strlen(const char *str);

/* Compares up to n characters of str1 and str2. */
int strncmp(const char *str1, const char *str2, uint n);

/* Copies up to n characters of str2 to str1. */
char *strncpy(char *dst, const char *src, uint n);

/* if c is a lowercase letter, returns its uppercase version */ 
char toupper(char c);

/* non-standard but useful */

/* Returns the lower of a and b. */
int min(int a, int b);

/* Locates the first occurrence of c in str within n characters. */
char *strnchr(const char *str, char c, uint n);

#endif
