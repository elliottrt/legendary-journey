#include "common/elf.h"
#include "common/std.h"
#include "common/file.h"
#include "boot/stage2/write.h"

bool stage2_elfread(struct file *file, void *dest, void (**entry)()) {

	struct elf_hdr *elfhdr = dest;
	struct elf_prog_hdr *proghdr, *endproghdr;

	if (!fileread(file, dest, sizeof(struct elf_hdr)))
		puterr("UNABLE TO READ ELF", 0);

	dest += sizeof(struct elf_hdr);

	proghdr = (struct elf_prog_hdr *)((uint8_t *) elfhdr + elfhdr->e_phoff);
	endproghdr = proghdr + elfhdr->e_phnum;

	if (elfhdr->e_magic != ELF_MAGIC)
		return false;

	if (fileread(file, dest, elfhdr->e_phnum * sizeof(struct elf_prog_hdr)) < 0)
		puterr("UNABLE TO READ ELF PROG HEADERS", 0);

	for (; proghdr < endproghdr; proghdr++) {
		uint8_t *address = (uint8_t *) proghdr->p_paddr;
		
		if (!fileseek(file, proghdr->p_offset))
			puterr("UNABLE TO SEEK SEGMENT", 0);
		if (!fileread(file, address, proghdr->p_filesz))
			puterr("UNABLE TO READ SEGMENT", 0);
		if (proghdr->p_filesz < proghdr->p_memsz)
			memset(address + proghdr->p_filesz, 0x00, proghdr->p_memsz - proghdr->p_filesz);
	}

	*entry = (void (*)()) elfhdr->e_entry;
	return true;
}
