#include "elf.h"
#include "write.h"
#include "std.h"

bool elfread(struct file *file, void *scratch, void (**entry)()) {

	struct elfheader *elfhdr = (struct elfheader *) scratch;
	struct elfprogheader *proghdr, *endproghdr;

	if (fileread(file, scratch, sizeof(struct elfheader)) < 0)
		puterr("UNABLE TO READ ELF", 0);

	scratch += sizeof(struct elfheader);

	proghdr = (struct elfprogheader *)((uint8_t *) elfhdr + elfhdr->phoff);
	endproghdr = proghdr + elfhdr->phnum;

	if (elfhdr->magic != ELF_MAGIC)
		return false;

	if (fileread(file, scratch, (void *) endproghdr - scratch) < 0)
		puterr("UNABLE TO READ ELF PROG HEADERS", 0);

	for (; proghdr < endproghdr; proghdr++)
	{
		uint8_t *address = (uint8_t *) proghdr->paddr;
		if (fileseek(file, proghdr->off) < 0)
			puterr("UNABLE TO SEEK SEGMENT", 0);
		if (fileread(file, address, proghdr->filesz) < 0)
			puterr("UNABLE TO READ SEGMENT", 0);
		if (proghdr->filesz < proghdr->memsz)
			memset((void *)(address + proghdr->filesz), 0x00, proghdr->memsz - proghdr->filesz);
	}

	*entry = (void (*)(uint32_t)) elfhdr->entry;
	return true;
}
