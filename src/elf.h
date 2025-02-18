#ifndef _ELF_FORMAT
#define _ELF_FORMAT

#include "types.h"
#include "file.h"

// See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format

#define ELF_MAGIC 0x464C457F

struct elf_hdr {
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

struct elf_prog_hdr {
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
} __attribute__ ((packed));

enum elf_prog_type {
	PT_NULL = 0,
	PT_LOAD = 1,
	PT_DYNAMIC = 2,
	PT_INTERP = 3,
	PT_NOTE = 4,
	PT_SHLIB = 5,
	PT_PHDR = 6,
	PT_TLS = 7,
	PT_LOOS = 0x60000000,
	PT_HIOS = 0x6FFFFFFF,
	PT_LOPROC = 0x70000000,
	PT_HIPROC = 0x7FFFFFFF
};

struct elf_sec_hdr {
	uint32_t sh_name;
	uint32_t sh_type;
	uint32_t sh_flags;
	uint32_t sh_addr;
	uint32_t sh_offset;
	uint32_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint32_t sh_addralign;
	uint32_t sh_entsize;
} __attribute__ ((packed));

enum elf_sec_type {
	SHT_NULL = 0,
	SHT_PROGBITS = 1,
	SHT_SYMTAB = 2,
	SHT_STRTAB = 3,
	SHT_RELA = 4,
	SHT_HASH = 5,
	SHT_DYNAMIC = 6,
	SHT_NOTE = 7,
	SHT_NOBITS = 8,
	SHT_REL = 9,
	SHT_SHLIB = 0xA,
	SHT_DYNSYM = 0xB,
	SHT_INIT_ARRAY = 0xE,
	SHT_FINI_ARRAY = 0xF,
	SHT_PREINIT_ARRAY = 0x10,
	SHT_GROUP = 0x11,
	SHT_SYMTAB_SHNDX = 0x12,
	SHT_NUM = 0x13,
	SHT_LOOS = 0x60000000
};

struct elf_rel {
	uint32_t r_offset;
	uint32_t r_info;
} __attribute__ ((packed));

#define ELF32_R_SYM(i)	(((i) >> 8) & 0xFF)
#define ELF32_R_TYPE(i)   ((i) & 0xFF)

struct elf_sym {
	uint32_t st_name; // index into symstrtab
	uint32_t st_value; // value of symbol, 
	uint32_t st_size; // size of data object, or 0
	uint8_t st_info; // bind and type info
	uint8_t st_other; // 0, no meaning
	uint16_t st_shndx; // index to relevant section header
} __attribute__ ((packed));

#define ELF32_ST_BIND(i) (((i)>>4) & 0xF)
#define ELF32_ST_TYPE(i) ((i) & 0xF)

#define SHN_UNDEF (0) // for st_shndx, denotes undefined symbol

// the signature for the main function of user programs
typedef int (*user_entry_t)(int, char **);

// used by stage2 to go to the kernel entry point
bool stage2_elfread(struct file *file, void *dest, void (**entry)());

// used by the kernel to load elf files
bool elfread(struct file *file, void *dest, user_entry_t *entry);

#endif
