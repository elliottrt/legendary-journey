#ifndef _STD
#define _STD

#include "types.h"
#include "x86.h"

#define NULL 0

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

/* non-standard but useful */

/* Returns the lower of a and b. */
int min(int a, int b);
uint umin(uint a, uint b);

/* Returns the higher of a and b */
int max(int a, int b);
uint umax(uint a, uint b);

/* Locates the first occurrence of c in str within n characters. */
char *strnchr(const char *str, char c, uint n);

#define HIBIT(_x) (31 - __builtin_clz((_x)))

// returns the lowest set bit of x
#define LOBIT(_x)\
    __extension__({ __typeof__(_x) __x = (_x); HIBIT(__x & -__x); })

// returns _v with _n-th bit = _x
#define BIT_SET(_v, _n, _x) __extension__({\
        __typeof__(_v) __v = (_v);\
        (__v ^ ((-(_x) ^ __v) & (1 << (_n))));\
        })

#endif
