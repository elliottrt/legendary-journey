#include "program.h"

#include "common/file.h"

user_entry_t program_load(const char *filename) {
	struct file program_file;
	user_entry_t program_entry = NULL;

	if (fileopen(&program_file, filename, 0)) {
		elfread(&program_file, (void *) USERBASE, &program_entry);
		fileclose(&program_file);
	}

	return program_entry;
}
