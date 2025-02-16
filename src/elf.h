#ifndef _ELF_FORMAT
#define _ELF_FORMAT

#include "file.h"
#include "types.h"

/*
See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
*/

#define ELF_MAGIC 0x464C457F

struct elfheader {
	uint32_t e_magic;
	uint8_t e_bits;
	uint8_t e_endian;
	uint8_t e_headerversion;
	uint8_t e_abi;
	uint8_t e_abiversion;
	uint8_t padding[7];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint32_t e_entry;
	uint32_t e_phoff;
	uint32_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
} __attribute__ ((packed));

struct elfprogheader {
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
} __attribute__ ((packed));

enum elfprogtype {
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

#endif
