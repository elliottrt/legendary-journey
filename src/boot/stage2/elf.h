#ifndef _STAGE2_ELF
#define _STAGE2_ELF

#include "file.h"
#include "types.h"

/*
See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
*/

#define ELF_MAGIC 0x464C457F

struct elfheader 
{
	uint magic;
	uchar bits;
    uchar endian;
    uchar headerversion;
    uchar abi;
    uchar abiversion;
    uchar padding[7];
    ushort type;
    ushort machine;
    uint version;
    uint entry;
    uint phoff;
    uint shoff;
    uint flags;
    ushort ehsize;
    ushort phentsize;
    ushort phnum;
    ushort shentsize;
    ushort shnum;
    ushort shstrndx;
} __attribute__ ((packed));

struct elfprogheader
{
    uint type;
	uint off;
	uint vaddr;
	uint paddr;
	uint filesz;
	uint memsz;
	uint flags;
	uint align;
} __attribute__ ((packed));

enum elfprogtype 
{
    ELF_PTYPE_NULL = 0,
    ELF_PTYPE_LOAD = 1,
    ELF_PTYPE_DYNAMIC = 2,
    ELF_PTYPE_INTERP = 3,
    ELF_PTYPE_NOTE = 4,
    ELF_PTYPE_SHLIB = 5,
    ELF_PTYPE_PHDR = 6,
    ELF_PTYPE_TLS = 7,
    ELF_PTYPE_LOOS = 0x60000000,
    ELF_PTYPE_HIOS = 0x6FFFFFFF,
    ELF_PTYPE_LOPROC = 0x70000000,
    ELF_PTYPE_HIPROC = 0x7FFFFFFF
};

int elfread(struct file *file, void *scratch, void (**entry)());

#endif
