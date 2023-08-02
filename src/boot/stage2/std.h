#ifndef _STAGE2_STD
#define _STAGE2_STD

#include <stdint.h>

#define true 1
#define false 0

#define NULL ((void *)0)

#define RAND_MAX UINT16_MAX

/* See https://www.ibm.com/docs/en/i/7.3?topic=extensions-standard-c-library-functions-table-by-name */
/* descriptions copied from there */

/* Calculates the absolute value of an integer argument n. */
int32_t abs(int32_t n);

/* Converts str to an int32_t. */
int32_t atoi(const char *str);

/* Tests if c is alphanumeric. */
int isalnum(char c);

/* Tests if c is alphabetic. */
int isalpha(char c);

/* Tests if c is a decimal digit. */
int isdigit(char c);

/* Tests if c is a lowercase letter. */
int islower(char c);

/* Tests if c is a whitespace character. */
int isspace(char c);

/* Tests if c is an uppercase letter. */
int isupper(char c);

/* Tests if c is a hexadecimal digit. */
int isxdigit(char c);

/* Searches the first n bytes of buf for the first occurrence of c. */
void *memchr(const void *buf, uint8_t c, uint32_t n);

/* Compares up to n bytes of buf1 and buf2. */
int32_t memcmp(const void *buf1, const void *buf2, uint32_t n);

/* Copies n bytes of src to dest. */
void *memcpy(void *dst, const void *src, uint32_t n);

/* Sets n bytes of dst to a value c. */
void *memset(void *dst, uint8_t c, uint32_t n);

/* Returns a pseudo-random uint32_t. */
uint32_t rand(void);

/* Sets the seed for the pseudo-random number generator. */
void srand(uint32_t seed);

/* Concatenates str2 to str1. */
char *strcat(char *dst, const char *src);

/* Locates the first occurrence of c in str. */
char *strchr(const char *str, char c);

/* Compares the value of str1 to str2. */
int32_t strcmp(const char *str1, const char *str2);

/* Copies str2 into str1. */
char *strcpy(char *dst, const char *src);

/* Calculates the length of str. */
uint32_t strlen(const char *str);

/* Concatenates up to n characters of str2 to str1. */
char *strncat(char *dst, const char *src, uint32_t n);

/* Compares up to n characters of str1 and str2. */
int32_t strncmp(const char *str1, const char *str2, uint32_t n);

/* Copies up to n characters of str2 to str1. */
char *strncpy(char *dst, const char *src, uint32_t n);

/* Returns a pointer to the first occurrence of str2 in str1. */
char *strstr(const char *str, const char *substr);

/* non-standard but useful */

/* Returns the lower of a and b. */
int32_t min(int32_t a, int32_t b);

/* Returns the higher of a and b. */
int32_t max(int32_t a, int32_t b);

/* Tests if two strings are equal. */
int streq(const char *str1, const char *str2);

/* sum up n bytes starting at addr */
uint32_t bytesum(void *addr, uint32_t n);

#endif
