#include "elf.h"
#include "std.h"
#include "file.h"
#include "memory/kalloc.h"
#include "user/sysfuncs.h"

#include "graphics/printf.h"

// see http://web.mit.edu/freebsd/head/sys/i386/i386/elf_machdep.c

bool elf_rel_patch(struct file *file, struct elf_sec_hdr *relhdr, struct elf_sym *symtab, const char *strtab) {

	if (!symtab || !strtab)
		return false;

	if (!fileseek(file, relhdr->sh_offset))
		return false;

	size_t entries = relhdr->sh_size / sizeof(struct elf_rel);
	struct elf_rel entry = {0};

	for (size_t i = 0; i < entries; i++) {
		if (fileread(file, &entry, sizeof(entry)) != sizeof(entry))
			return false;

		uint32_t *loc = (void *) entry.r_offset;

		struct elf_sym *sym = symtab + ELF32_R_SYM(entry.r_info);
		const char *name = strtab + sym->st_name;

		/*
		printf("rel patching: offset=0x%8x, type=0x%2x, sym=0x%6x, addend=0x%8x\n",
			entry.r_offset,
			ELF32_R_TYPE(entry.r_info),
			ELF32_R_SYM(entry.r_info),
			*loc
		);
		*/

		switch (ELF32_R_TYPE(entry.r_info)) {
			case 1: { // R_386_32

				// because the linker fixes up defined symbols, all entries of this
				// type SHOULD be defined. if they aren't, error
				if (*loc == 0) {
					printf("error: elf: undefined symbol '%s'\n", name);
					errno = ENOSYS;
					return false;
				}

			} break;
			case 2: { // R_386_PC32

				// if symbol is undefined
				if (sym->st_shndx == SHN_UNDEF) {
					// try to get a system function if symbol is 0
					sym->st_value = sysfunc_get(name);
					
					// if couldn't get system func, then error
					if (sym->st_value == 0) {
						printf("error: elf: undefined symbol '%s'\n", name);
						errno = ENOSYS;
						return false;
					}
				}

				// ??? why is it 4 off ???
				// see https://stackoverflow.com/questions/50357270/elf-understanding-r-386-pc32-relocations
				// TODO: it might not always be '-4', so try to figure out exactly what to do
				*loc = sym->st_value - entry.r_offset - 4;
			} break;
			default: break;
		}

		// printf("    was patched to 0x%8x\n", *loc);
	}

	return true;
}

bool read_elf_header(struct file *file, void *dest) {
	if (!fileseek(file, 0)) 
		return false;

	if (!fileread(file, dest, sizeof(struct elf_hdr)))
		return false;

	if (((struct elf_hdr *) dest)->e_magic != ELF_MAGIC)
		return false;

	return true;
}

bool load_program_header(struct file *file, struct elf_prog_hdr *header) {
	uint8_t *address = (uint8_t *) header->p_vaddr;
	
	if (!fileseek(file, header->p_offset))
		return false;

	if (fileread(file, address, header->p_filesz) != (int32_t) header->p_filesz)
		return false;
	if (header->p_filesz < header->p_memsz)
		memset(address + header->p_filesz, 0x00, header->p_memsz - header->p_filesz);

	return true;
}

bool read_program_headers(struct file *file, struct elf_hdr *header, void *dest) {
	return fileseek(file, header->e_phoff) &&
		fileread(file, dest, sizeof(struct elf_prog_hdr) * header->e_phnum) > 0;
}

bool read_section_headers(struct file *file, struct elf_hdr *header, void *dest) {
	return fileseek(file, header->e_shoff) &&
		fileread(file, dest, sizeof(struct elf_sec_hdr) * header->e_shnum) > 0;
}

void *read_section(struct file *file, struct elf_sec_hdr *section) {
	void *data = NULL;

	if (!fileseek(file, section->sh_offset))
		return NULL;

	if (section->sh_size > PGSIZE) {
		errno = ENOMEM;
		return NULL;
	}
 	
	if ((data = kalloc()) == NULL) {
		return NULL;
	}

	if (fileread(file, data, section->sh_size) != (int32_t) section->sh_size) {
		kfree(data);
		return NULL;
	}

	return data;
}

bool elfread(struct file *file, void *dest, user_entry_t *entry) {
	UNUSED(dest);

	// success?
	bool result = true;
	// elf program header
	struct elf_hdr elfhdr = {0};
	// points to the first program header
	struct elf_prog_hdr *proghdr = NULL;
	// points to the first section header
	struct elf_sec_hdr *sechdr = NULL;
	// points to the symbol table
	struct elf_sym *symtab = NULL;
	// points to the string table (not the shstrtab)
	const char *strtab = NULL;

	// read elf header

	if (!read_elf_header(file, &elfhdr)) {
		result = false;
		goto done;
	}

	// we may not have enough memory in a single kalloc() to store
	// the headers. in that case, return error to signify

	if (elfhdr.e_phnum * sizeof(struct elf_prog_hdr) > PGSIZE) {
		result = false;
		errno = ENOMEM;
		goto done;
	}
	if (elfhdr.e_shnum * sizeof(struct elf_sec_hdr) > PGSIZE) {
		result = false;
		errno = ENOMEM;
		goto done;
	}
	
	// allocate space for program and section headers
	if ((proghdr = kalloc()) == NULL) {
		result = false;
		goto done;
	}

	if ((sechdr = kalloc()) == NULL) {
		result = false;
		goto done;
	}
	
	// seek to and read the program headers 
	
	if (!read_program_headers(file, &elfhdr, proghdr)) {
		result = false;
		goto done;
	}

	// seek to and read the section headers
	if (!read_section_headers(file, &elfhdr, sechdr)) {
		result = false;
		goto done;
	}

	// do initialization of required tables - symtab, etc.
	for (struct elf_sec_hdr *s = sechdr; s < sechdr + elfhdr.e_shnum; s++) {
		switch (s->sh_type) {
			case SHT_SYMTAB: { // store symbol table address for possible rel patching
				if ((symtab = read_section(file, s)) == NULL) {
					result = false;
					goto done;
				}
			} break;
			case SHT_STRTAB: {

				if (s - sechdr == elfhdr.e_shstrndx) {
					// TODO: found shstrtab, do something?
				} else {
					if ((strtab = read_section(file, s)) == NULL) {
						result = false;
						goto done;
					}
				}

			} break;
			default: break;
		}
	}

	// load program info into memory
	for (struct elf_prog_hdr *p = proghdr; p < proghdr + elfhdr.e_phnum; p++) {
		if (!load_program_header(file, p)) {
			result = false;
			goto done;
		}
	}

	// second loop, do things that require tables and program
	for (struct elf_sec_hdr *s = sechdr; s < sechdr + elfhdr.e_shnum; s++) {
		switch (s->sh_type) {
			case SHT_REL: {
				if (!elf_rel_patch(file, s, symtab, strtab)) {
					result = false;
					goto done;
				}
			} break;
			default: break;
		}
	}

done:
	kfree(proghdr);
	kfree(sechdr);
	kfree(symtab);

	*entry = (user_entry_t) elfhdr.e_entry;
	return result;
}
