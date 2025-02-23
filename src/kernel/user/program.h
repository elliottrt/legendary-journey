#ifndef _PROGRAM
#define _PROGRAM

#include "common/elf.h"
#include "common/file.h"

// TODO: is this the solution?
#define PROGRAM_MAX_FILES 7

struct program_data {
	struct file files[PROGRAM_MAX_FILES];

	uintptr_t address; // where does this start in virtual memory?
	size_t pages; // how many pages does this use?

	user_entry_t entry; // entry point into program
};

_Static_assert(sizeof(struct program_data) <= PGSIZE, "sizeof(struct program_data) must be <= PGSIZE");

user_entry_t program_load(const char *filename);

#endif
