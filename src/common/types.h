#ifndef _TYPES
#define _TYPES

#define NULL ((void*)0)

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

typedef unsigned int bool;
#define true (1)
#define false (0)

struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_no;
    uint32_t eip, cs, eflags, useresp, ss;
};

struct memregion {
    uint64_t address;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
};

#define BIT(n) (1 << (n))

#endif
