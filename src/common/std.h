#ifndef _STD
#define _STD

#include "types.h"
#include "x86.h"

#define UNUSED(X) (void)(X)

enum errno {
	ENOENT = 1, 	/* no such file or directory */
	EBADF, 			/* bad file descriptor */
	ENOTDIR, 		/* tried to read something not a directory as a directory */
	EISDIR, 		/* is a directory */
	EINVAL, 		/* invalid argument */
	EPATH, 			/* bad file path */
	EEXIST, 		/* file already exists */
	EACCES, 		/* permission denied */
	EIO, 			/* general io error */
	ENAMETOOLONG, 	/* filename too long */
	ENOMEM, 		/* not enough space / can't allocate memory */
	ENOSYS, 		/* function not implemented */
	ENOTEMPTY, 		/* directory not empty */
	EPERM, 			/* operation not permitted */
	ENOEXEC,		/* exec format error */
	E2BIG,			/* argument list too long */
	ENOSPC,			/* no space left on device */
	EMFILE,			/* too many files open */

	ERRNO_COUNT		/* errno values defined */
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

/* Copies n bytes of src to dest, safe for overlapping blocks. */
void *memmove(void *dst, const void *src, uint32_t n);

/* Locates the first occurrence of c in str. */
char *strchr(const char *str, char c);

/* Calculates the length of str. */
uint32_t strlen(const char *str);

/* Lexicographically compares str1 and str2 */
int strcmp(const char *str1, const char *str2);

/* Compares up to n characters of str1 and str2. */
int strncmp(const char *str1, const char *str2, uint32_t n);

/* Copies up to n characters of str2 to str1. */
char *strncpy(char *dst, const char *src, uint32_t n);

bool isdigit(int ch);
bool isxdigit(int ch);
bool isascii(int ch);
bool isalnum(int ch);
bool islower(int ch);
bool isupper(int ch);
bool isspace(int ch);

/* non-standard but useful */

/* Returns the lower of a and b. */
int32_t min(int32_t a, int32_t b);
uint32_t umin(uint32_t a, uint32_t b);

/* Returns the higher of a and b */
int32_t max(int32_t a, int32_t b);
uint32_t umax(uint32_t a, uint32_t b);

/* Locates the first occurrence of c in str within n characters. */
char *strnchr(const char *str, char c, uint32_t n);

// returns V with N-th bit = X
#define BIT_SET(V, N, X) (V ^ ((-(X) ^ V) & (1 << (N))))

#endif
