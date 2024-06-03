#ifndef _TYPES
#define _TYPES

#define NULL ((void*)0)

// TODO: size_t
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef int bool;
#define true 1
#define false 0

struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_no;
    uint32_t eip, cs, eflags, useresp, ss;
};

#define BIT(n) (1 << (n))

#endif
