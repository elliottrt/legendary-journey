#include "std.h"

int32_t abs(int32_t n)
{
	return n >= 0 ? n : -n;
}

int32_t atoi(const char *str)
{
	char *current = (char *) str;
	int32_t result = 0;
	int negative = false;

	if (*current == '-')
	{
		negative = true;
		current++;
	}

	while (*current != '\0')
	{
		result = (result * 10) + (*current - '0');
		current++;
	}

	if (negative)
		result = -result;

	return result;
}

int isalnum(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isalpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isdigit(char c)
{
	return c >= '0' && c <= '9';
}

int islower(char c)
{
	return c >= 'a' && c <= 'z';
}

int isspace(char c)
{
	return c == ' '  ||
		   c == '\f' ||
		   c == '\n' ||
		   c == '\r' ||
		   c == '\t' ||
		   c == '\v';
}

int isupper(char c)
{
	return c >= 'A' && c <= 'Z';
}

int isxdigit(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

void *memchr(const void *buf, uint8_t c, uint32_t n)
{
	uint8_t *_buf = (uint8_t *) buf;

	for (; n--; _buf++)
	{
		if (*_buf == c)
			return _buf;
	}

	return NULL;
}

int32_t memcmp(const void *buf1, const void *buf2, uint32_t n)
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

uint32_t _seed = 1;
/* From https://stackoverflow.com/questions/4768180/rand-implementation */
uint32_t rand(void)
{
	_seed = _seed * 1103515245 + 12345;
	return (_seed / (RAND_MAX * 2)) % RAND_MAX;
}

void srand(uint32_t seed)
{
	_seed = seed;
}

char *strcat(char *str1, const char *str2)
{
	char *dst = str1 + strlen(str1);

	for (; *str2; dst++, str2++)
		*dst = *str2;
	
	return str1;
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

int32_t strcmp(const char *str1, const char *str2)
{
	while(*str1 && *str2 && *str1 == *str2)
	{
		str1++;
		str2++;
	}
	return *str1 - *str2;
}

char *strcpy(char *str1, const char *str2)
{
	char *str1_start = str1;

	for (; *str2; str1++, str2++)
		*str1 = *str2;

	return str1_start;
}

uint32_t strlen(const char *str)
{
	uint32_t len = 0;
	while(str[len++]);
	return len - 1;
}

char *strncat(char *str1, const char *str2, uint32_t n)
{
	char *dst = str1 + strlen(str1);

	for (; *str2 && n--; dst++, str2++)
		*dst = *str2;
	
	return str1;
}

int32_t strncmp(const char *str1, const char *str2, uint32_t n)
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

char *strstr(const char *str1, const char *str2)
{
	uint32_t len1 = strlen(str1);
	uint32_t len2 = strlen(str2);

	const char *end = str1 + len1 - len2;

	for (; str1 < end; str1++)
	{
		if (*str1 == *str2 && memcmp(str1, str2, len2) == 0)
			return (char *) str1;
	}

	return NULL;
}

int32_t min(int32_t a, int32_t b)
{
	return a > b ? b : a;
}

int32_t max(int32_t a, int32_t b)
{
	return a > b ? a : b;
}

int streq(const char *str1, const char *str2)
{
	while(*str1 && *str2 && *str1 == *str2)
	{
		str1++;
		str2++;
	}
	return (*str1 - *str2) == 0;
}

uint32_t bytesum(void *addr, uint32_t n)
{
	uint32_t sum;
	uint8_t *bytes = (uint8_t *) addr;
	for (sum = 0; n--; sum += *bytes, bytes++);
	return sum;
}
