#include "common/std.h"

enum errno errno;

const char *strerror(enum errno _errno) {
	static const char *strs[] = {
		"no error",
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
		"operation not permitted",
		"exec format error",
		"argument list too long",
		"no space left on device",
		"too many open files",
	};

	_Static_assert(sizeof(strs) / sizeof(*strs) == ERRNO_COUNT, "must have a string for each errno");

	return strs[_errno];
}

int memcmp(const void *buf1, const void *buf2, uint32_t n) {
	uint8_t *left = (uint8_t *) buf1;
	uint8_t *right = (uint8_t *) buf2;

	for (; n--; left++, right++)
	{
		if (*left != *right)
			return (*left) - (*right);
	}

	return 0;
}

void *memcpy(void *dst, const void *src, uint32_t n) {
	uint8_t *_dst = (uint8_t *) dst;
	uint8_t *_src = (uint8_t *) src;

	for (; n--; _dst++, _src++)
		*_dst = *_src;

	return dst;
}

void *memset(void *dst, uint8_t c, uint32_t n) {
	uint8_t *_dst = (uint8_t *) dst;

	for (; n--; _dst++)
		*_dst = c;

	return dst;
}

// implementation referenced from https://github.com/drh/lcc/blob/2b5cf358d9aa6759923dd7461f2df7f7f2a28471/cpp/unix.c#L96
void *memmove(void *_dst, const void *_src, uint32_t n) {
	uint8_t *dst = _dst;
	const uint8_t *src = _src;

	if (dst < src) {
		do {
			*dst++ = *src++;
		} while (--n);
	} else { // dst > src
		dst += n;
		src += n;
		do {
			*--dst = *--src;
		} while (--n);
	}

	return _dst;
}

char *strchr(const char *str, char c) {
	for (; *str; str++)
	{
		if (*str == c)
			return (char *) str;
	}

	return NULL;
}

uint32_t strlen(const char *str) {
	uint32_t len = 0;
	while(str[len++]);
	return len - 1;
}

// TODO: test
int strcmp(const char *str1, const char *str2) {
	for (; *str1 && *str2; str1++, str2++) {
		if (*str1 != *str2) return (int) *str2 - (int) *str1;
	}
	return (int) *str2 - (int) *str1;
}

int strncmp(const char *str1, const char *str2, uint32_t n) {
	while(*str1 && *str2 && *str1 == *str2 && --n)
	{
		str1++;
		str2++;
	}
	return *str1 - *str2;
}

char *strncpy(char *str1, const char *str2, uint32_t n) {
	char *str1_start = str1;

	for (; *str2 && n--; str1++, str2++)
		*str1 = *str2;

	return str1_start;
}

char *strnchr(const char *str, char c, uint32_t n) {
	for (; *str && n--; str++) {
		if (*str == c)
			return (char *) str;
	}

	return NULL;
}

bool isascii(unsigned char ch) {
	return (ch & 0x80) == 0;
}

bool isdigit(int ch) {
	return ch >= '0' && ch <= '9';
}

bool isxdigit(int ch) {
	return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

bool isalpha(int ch) {
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

bool isalnum(int ch) {
	return isalpha(ch) || isdigit(ch);
}

bool islower(int ch) {
	return ch >= 'a' && ch <= 'z';
}

bool isupper(int ch) {
	return ch >= 'A' && ch <= 'Z';
}

bool isspace(int ch) {
	return 	ch == ' ' ||
			ch == '\f' ||
			ch == '\n' ||
			ch == '\r' ||
			ch == '\t' ||
			ch == '\v';
}

bool isprint(int ch) {
	// implemented according to the table here: https://cppreference.com/w/c/string/byte/isprint.html

	return 32 <= ch && ch <= 126;
}

int32_t min(int32_t a, int32_t b) {
	return a > b ? b : a;
}

uint32_t umin(uint32_t a, uint32_t b) {
	return a > b ? b : a;
}

int32_t max(int32_t a, int32_t b) {
	return a > b ? a : b;
}

uint32_t umax(uint32_t a, uint32_t b) {
	return a > b ? a : b;
}
