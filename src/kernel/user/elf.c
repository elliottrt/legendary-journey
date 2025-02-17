#include "elf.h"
#include "std.h"
#include "file.h"

#include "graphics/printf.h"

bool elf_get_shname(struct file *file, uint32_t sh_offset, uint32_t offset, char *out, uint32_t outlen) {
	// go to location where string starts
	if (!fileseek(file, sh_offset + offset)) return false;

	for (uint32_t i = 0; i < outlen; i++) {
		// read one char
		if (fileread(file, out + i, sizeof(*out)) != sizeof(*out)) return false;
		if (out[i] == '\0') break;
	}

	out[outlen - 1] = '\0';
	return true;
}

bool elfread(struct file *file, void *dest, user_entry_t *entry) {

	struct elfheader *elfhdr = (struct elfheader *) dest;
	// points to the first program header
	struct elfprogheader *proghdr;
	// points to the end of the last program header
	struct elfprogheader *endproghdr;

	printf("spot 0\n");

	if (!fileread(file, dest, sizeof(struct elfheader)))
		return false;

	dest += sizeof(struct elfheader);

	proghdr = (struct elfprogheader *)((uint8_t *) elfhdr + elfhdr->e_phoff);
	endproghdr = proghdr + elfhdr->e_phnum;

	printf("spot 1, %d prog header(s)\n", elfhdr->e_phnum);

	if (elfhdr->e_magic != ELF_MAGIC)
		return false;

	if (fileread(file, dest, sizeof(struct elfprogheader) * elfhdr->e_phnum) < 0)
		return false;

	printf("spot 1.5\n");

	// TODO: remove this debug info
	struct elfsectionheader shstrtab = {0};
	fileseek(file, elfhdr->e_shoff + elfhdr->e_shstrndx * sizeof(struct elfsectionheader));
	fileread(file, &shstrtab, sizeof(shstrtab));


	struct elfsectionheader sechdr = {0};
	fileseek(file, elfhdr->e_shoff);
	char namebuf[32] = {0};
	for (int i = 0; i < elfhdr->e_shnum; i++) {
		fileseek(file, elfhdr->e_shoff + i * sizeof(struct elfsectionheader));
		fileread(file, &sechdr, sizeof(sechdr));
		if (!elf_get_shname(file, shstrtab.sh_offset, sechdr.sh_name, namebuf, sizeof(namebuf)))
			return false;
		printf("sechdr: %s, size=%u\n", namebuf, sechdr.sh_size);
	}

	printf("spot 2\n");

	for (; proghdr < endproghdr; proghdr++) {
		uint8_t *address = (uint8_t *) proghdr->p_vaddr;

		printf("spot 3\n");
		
		if (!fileseek(file, proghdr->p_offset))
			return false;

		printf("loading type %d at 0x%x (%u bytes)\n", proghdr->p_type, address, proghdr->p_filesz);

		if (fileread(file, address, proghdr->p_filesz) != proghdr->p_filesz)
			return false;
		if (proghdr->p_filesz < proghdr->p_memsz)
			memset(address + proghdr->p_filesz, 0x00, proghdr->p_memsz - proghdr->p_filesz);
	}

	printf("spot 4\n");

	*entry = (user_entry_t) elfhdr->e_entry;
	return true;
}
