#include "elf.h"
#include "write.h"
#include "std.h"

int elfread(struct file *file, void *scratch, void (**entry)(uint))
{

	struct elfheader *elfhdr = (struct elfheader *) scratch;
	struct elfprogheader *proghdr, *endproghdr;

	if (fileread(file, scratch, sizeof(struct elfheader)) < 0)
		puterr("UNABLE TO READ ELF", 0);

	scratch += sizeof(struct elfheader);

	proghdr = (struct elfprogheader *)((uchar *) elfhdr + elfhdr->phoff);
	endproghdr = proghdr + elfhdr->phnum;

	if (elfhdr->magic != ELF_MAGIC)
		return -1;

	if (fileread(file, scratch, (void *) endproghdr - scratch) < 0)
		puterr("UNABLE TO READ ELF PROG HEADERS", 0);

	for (; proghdr < endproghdr; proghdr++)
	{
		uchar *address = (uchar *) proghdr->paddr;
		if (fileseek(file, proghdr->off) < 0)
			puterr("UNABLE TO SEEK SEGMENT", 0);
		if (fileread(file, address, proghdr->filesz) < 0)
			puterr("UNABLE TO READ SEGMENT", 0);
		if (proghdr->filesz < proghdr->memsz)
			memset((void *)(address + proghdr->filesz), 0x00, proghdr->memsz - proghdr->filesz);
	}

	*entry = (void (*)(uint)) elfhdr->entry;
	return 0;
}
