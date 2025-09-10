#ifndef LEGENDARY_JOURNEY_STDLIB
#define LEGENDARY_JOURNEY_STDLIB

// TODO: documentation
// TODO: way to iterate over a directory's children

#define NULL ((void*)0)

// standard sized integer types (stdint.h)

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef int32_t ssize_t;
typedef uint32_t size_t;
typedef uint32_t uintptr_t;

_Static_assert(sizeof(int8_t) == 1, "int8_t must be 1 byte");
_Static_assert(sizeof(uint8_t) == 1, "uint8_t must be 1 byte");
_Static_assert(sizeof(int16_t) == 2, "int16_t must be 2 bytes");
_Static_assert(sizeof(uint16_t) == 2, "uint16_t must be 2 bytes");
_Static_assert(sizeof(int32_t) == 4, "int32_t must be 4 bytes");
_Static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");
_Static_assert(sizeof(int64_t) == 8, "int64_t must be 8 bytes");
_Static_assert(sizeof(uint64_t) == 8, "uint64_t must be 8 bytes");

_Static_assert(sizeof(uintptr_t) == sizeof(void *), "uintptr_t must the same size of a void *");

// standard boolean definitions (stdbool.h)

typedef unsigned int bool;
#define true (1)
#define false (0)

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// program current directory interface

extern const char *getcwd(void);
extern int setcwd(const char *path);

// stdio access

extern int printf(const char *format, ...);
extern int putchar(int ch);
extern int puts(const char *str);

// timer interface

extern uint32_t timer_read(void);

extern void timer_wait_until(uint32_t time);

extern void timer_wait(uint32_t ms);

// file access interface

enum fflags {
	FTRUNC = 1 << 0, /* open file at start */
	FAPPEND = 1 << 1, /* open file at end */
	FCREATE = 1 << 2, /* create a file if none exists */
	FDIRECTORY = 1 << 3, /* open a dir, or create a dir if FCREATE set */
};
typedef void FILE;

extern FILE *fopen(const char *path, int flags);

extern int fclose(FILE *fp);

extern size_t fread(void *buffer, size_t size, size_t count, FILE *fp);

extern size_t fwrite(const void *buffer, size_t size, size_t count, FILE *fp);

extern long ftell(FILE *fp);

extern int fseek(FILE *fp, uint32_t offset);

extern int fflush(FILE *fp);

extern void frewind(FILE *fp);

extern int fresize(FILE *fp, uint32_t size);

extern uint32_t fsize(FILE *fp);

// memory allocation interface

extern void *malloc(size_t size);

extern void free(void *ptr);

#endif
