#include "program.h"

#include "common/file.h"

#include "kernel/graphics/printf.h"

// path used in loading programs
char program_path[PATH_MAX] = {0};

user_entry_t program_load(const char *filename) {
	struct file program_file;
	user_entry_t program_entry = NULL;

	if (fileopen(&program_file, filename, 0)) {
		elfread(&program_file, (void *) USERBASE, &program_entry);
		fileclose(&program_file);
	}

	return program_entry;
}

struct program_data *user_mode_start(const char *path, const char *filename) {
	struct program_data *data = rm_alloc(rm_global_ctx, sizeof(*data));
	if (!data) return NULL;

	// copy shell's current path into the user program
	if (path_copy(path, data->dir, sizeof(data->dir)) < 0) {
		rm_free(rm_global_ctx, data);
		return NULL;
	}

	// try loading program
	if (path_concat(path, filename, program_path, sizeof(program_path)) < 0) {
		rm_free(rm_global_ctx, data);
		return NULL;
	}

	data->entry = program_load(program_path);
	if (!data->entry) {
		rm_free(rm_global_ctx, data);
		return NULL;
	}

	void *malloc_block = rm_alloc(rm_global_ctx, PROGRAM_MALLOC_SIZE);
	if (!malloc_block) {
		rm_free(rm_global_ctx, data);
		return NULL;
	}

	data->malloc_context = rm_init_block(malloc_block, PROGRAM_MALLOC_SIZE);
	return data;
}

void user_mode_end(struct program_data *data) {
	rm_free(rm_global_ctx, data->malloc_context);
	rm_free(rm_global_ctx, data);
}
