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
	uint32_t magic;
	uint8_t bits;
    uint8_t endian;
    uint8_t headerversion;
    uint8_t abi;
    uint8_t abiversion;
    uint8_t padding[7];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} __attribute__ ((packed));

struct elfprogheader
{
    uint32_t type;
	uint32_t off;
	uint32_t vaddr;
	uint32_t paddr;
	uint32_t filesz;
	uint32_t memsz;
	uint32_t flags;
	uint32_t align;
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

bool elfread(struct file *file, void *scratch, void (**entry)());

#endif
