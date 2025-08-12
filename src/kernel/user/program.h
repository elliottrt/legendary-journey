#ifndef _PROGRAM
#define _PROGRAM

#include "common/elf.h"
#include "common/file.h"
#include "kernel/memory/malloc.h"

#define PROGRAM_MAX_FILES (4)
#define PROGRAM_MALLOC_SIZE (0x1000)

struct program_data {
	// files the program has open
	struct file files[PROGRAM_MAX_FILES];

	// main() function of the user program
	user_entry_t entry;

	// user program's memory allocation space
	rm_ctx_t *malloc_context;
};

user_entry_t program_load(const char *filename);

struct program_data *user_mode_start(const char *filename);
void user_mode_end(struct program_data *data);

#endif
