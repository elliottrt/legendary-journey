#include "std.h"

enum errno errno;

const char *strerror(enum errno _errno) {
	static const char *strs[] = {
		"no such file or directory",
		"bad file descriptor",
		"not directory",
		"is a directory",
		"invalid argument",
		"bad file path",
		"file already exists",
		"permission denied",
		"io error",
		"name too long",
		"not enough space",
		"function not implemented",
		"directory not empty",
		"operation not permitted"
	};
	return strs[_errno];
}

int memcmp(const void *buf1, const void *buf2, uint n)
{
	uchar *left = (uchar *) buf1;
	uchar *right = (uchar *) buf2;

	for (; n--; left++, right++)
	{
		if (*left != *right)
			return (*left) - (*right);
	}

	return 0;
}

void *memcpy(void *dst, const void *src, uint n)
{
	uchar *_dst = (uchar *) dst;
	uchar *_src = (uchar *) src;

	for (; n--; _dst++, _src++)
		*_dst = *_src;

	return dst;
}

void *memset(void *dst, uchar c, uint n)
{
	uchar *_dst = (uchar *) dst;

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

uint strlen(const char *str)
{
	uint len = 0;
	while(str[len++]);
	return len - 1;
}

int strncmp(const char *str1, const char *str2, uint n)
{
	while(*str1 && *str2 && *str1 == *str2 && --n)
	{
		str1++;
		str2++;
	}
	return *str1 - *str2;
}

char *strncpy(char *str1, const char *str2, uint n)
{
	char *str1_start = str1;

	for (; *str2 && n--; str1++, str2++)
		*str1 = *str2;

	return str1_start;
}

int min(int a, int b) {
	return a > b ? b : a;
}

uint umin(uint a, uint b) {
	return a > b ? b : a;
}

int max(int a, int b) {
	return a > b ? a : b;
}

uint umax(uint a, uint b) {
	return a > b ? a : b;
}

char *strnchr(const char *str, char c, uint n)
{
	for (; *str && n--; str++)
	{
		if (*str == c)
			return (char *) str;
	}

	return NULL;
}
