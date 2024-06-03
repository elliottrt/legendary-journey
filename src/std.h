#ifndef _STD
#define _STD

#include "types.h"
#include "x86.h"

#define UNUSED(X) (void)(X)

enum errno {
	ENOENT = 0, /* no such file or directory */
	EBADF = 1, /* bad file descriptor */
	ENOTDIR = 2, /* tried to read something not a directory as a directory */
	EISDIR = 3, /* is a directory */
	EINVAL = 4, /* invalid argument */
	EPATH = 5, /* bad file path */
	EEXIST = 6, /* file already exists */
	EACCES = 7, /* permission denied */
	EIO = 8, /* general io error */
	ENAMETOOLONG = 9, /* filename too long */
	ENOMEM = 10, /* not enough space / can't allocate memory */
	ENOSYS = 11, /* function not implemented */
	ENOTEMPTY = 12, /* directory not empty */
	EPERM = 13, /* operation not permitted */
};

extern enum errno errno;

const char *strerror(enum errno _errno);

/* See https://www.ibm.com/docs/en/i/7.3?topic=extensions-standard-c-library-functions-table-by-name */
/* descriptions copied from there */

/* Compares up to n bytes of buf1 and buf2. */
int memcmp(const void *buf1, const void *buf2, uint32_t n);

/* Copies n bytes of src to dest. */
void *memcpy(void *dst, const void *src, uint32_t n);

/* Sets n bytes of dst to a value c. */
void *memset(void *dst, uint8_t c, uint32_t n);

/* Locates the first occurrence of c in str. */
char *strchr(const char *str, char c);

/* Calculates the length of str. */
uint32_t strlen(const char *str);

/* Compares up to n characters of str1 and str2. */
int strncmp(const char *str1, const char *str2, uint32_t n);

/* Copies up to n characters of str2 to str1. */
char *strncpy(char *dst, const char *src, uint32_t n);

/* non-standard but useful */

/* Returns the lower of a and b. */
int32_t min(int32_t a, int32_t b);
uint32_t umin(uint32_t a, uint32_t b);

/* Returns the higher of a and b */
int32_t max(int32_t a, int32_t b);
uint32_t umax(uint32_t a, uint32_t b);

/* Locates the first occurrence of c in str within n characters. */
char *strnchr(const char *str, char c, uint32_t n);

// returns _v with _n-th bit = _x
#define BIT_SET(_v, _n, _x) __extension__({\
        __typeof__(_v) __v = (_v);\
        (__v ^ ((-(_x) ^ __v) & (1 << (_n))));\
        })

#endif
