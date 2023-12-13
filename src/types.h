#ifndef _TYPES
#define _TYPES

#define NULL 0

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;

struct registers {
    uint ds;
    uint edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint int_no, err_no;
    uint eip, cs, eflags, useresp, ss;
};

#endif
