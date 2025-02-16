#include "elf.h"
#include "write.h"
#include "std.h"

bool elfread(struct file *file, void *dest, void (**entry)()) {

	struct elfheader *elfhdr = (struct elfheader *) dest;
	struct elfprogheader *proghdr, *endproghdr;

	if (fileread(file, dest, sizeof(struct elfheader)) < 0)
		puterr("UNABLE TO READ ELF", 0);

	dest += sizeof(struct elfheader);

	proghdr = (struct elfprogheader *)((uint8_t *) elfhdr + elfhdr->e_phoff);
	endproghdr = proghdr + elfhdr->e_phnum;

	if (elfhdr->e_magic != ELF_MAGIC)
		return false;

	if (fileread(file, dest, (void *) endproghdr - dest) < 0)
		puterr("UNABLE TO READ ELF PROG HEADERS", 0);

	for (; proghdr < endproghdr; proghdr++) {
		uint8_t *address = (uint8_t *) proghdr->p_paddr;
		
		if (fileseek(file, proghdr->p_offset) < 0)
			puterr("UNABLE TO SEEK SEGMENT", 0);
		if (fileread(file, address, proghdr->p_filesz) < 0)
			puterr("UNABLE TO READ SEGMENT", 0);
		if (proghdr->p_filesz < proghdr->p_memsz)
			memset(address + proghdr->p_filesz, 0x00, proghdr->p_memsz - proghdr->p_filesz);
	}

	*entry = (void (*)()) elfhdr->e_entry;
	return true;
}
