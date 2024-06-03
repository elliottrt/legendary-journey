#include "std.h"

int memcmp(const void *buf1, const void *buf2, uint32_t n)
{
	uint8_t *left = (uint8_t *) buf1;
	uint8_t *right = (uint8_t *) buf2;

	for (; n--; left++, right++)
	{
		if (*left != *right)
			return (*left) - (*right);
	}

	return 0;
}

void *memcpy(void *dst, const void *src, uint32_t n)
{
	uint8_t *_dst = (uint8_t *) dst;
	uint8_t *_src = (uint8_t *) src;

	for (; n--; _dst++, _src++)
		*_dst = *_src;

	return dst;
}

void *memset(void *dst, uint8_t c, uint32_t n)
{
	uint8_t *_dst = (uint8_t *) dst;

	for (; n--; _dst++)
		*_dst = c;

	return dst;
}

char *strchr(const char *str, char c)
{
	for (; *str; str++)
	{
		if (*str == c)
			return (char *) str;
	}

	return NULL;
}

uint32_t strlen(const char *str)
{
	uint32_t len = 0;
	while(str[len++]);
	return len - 1;
}

int strncmp(const char *str1, const char *str2, uint32_t n)
{
	while(*str1 && *str2 && *str1 == *str2 && --n)
	{
		str1++;
		str2++;
	}
	return *str1 - *str2;
}

char *strncpy(char *str1, const char *str2, uint32_t n)
{
	char *str1_start = str1;

	for (; *str2 && n--; str1++, str2++)
		*str1 = *str2;

	return str1_start;
}

int32_t min(int32_t a, int32_t b)
{
	return a > b ? b : a;
}

char *strnchr(const char *str, char c, uint32_t n)
{
	for (; *str && n--; str++)
	{
		if (*str == c)
			return (char *) str;
	}

	return NULL;
}
