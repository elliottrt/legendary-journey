#include "kernel/user/user_functions.h"
#include "common/std.h"

#include "kernel/graphics/printf.h"
#include "kernel/drivers/timer.h"
#include "common/file.h"

#define SYSDEF_MAX_LEN (16)

struct user_function_def {
	char name[SYSDEF_MAX_LEN];
	uintptr_t func;
};

static struct program_data *user_data;
char temp_path[PATH_MAX] = DEFAULT_PATH;

// load a path into temp_path, using user_data->dir if path is not absolute
static int load_path(const char *path) {
	if (!path) {
		errno = EINVAL;
		return -1;
	}

	// check if absolute path
	if (*path == '/') {
		// load straight into temp_path
		// TODO: do we need to simplify this path?
		return path_copy(path, temp_path, PATH_MAX);
	} else {
		// relative path, concat with user path
		return path_concat(user_data->dir, path, temp_path, PATH_MAX);
	}
}

static const char *getcwd(void) {
	return user_data->dir;
}

static int setcwd(const char *path) {
	// put the path into temp
	if (load_path(path) < 0) return -1;

	// make sure the new path exists, is a directory, etc.
	if (path_exists(temp_path, 1) <= 0) return -1;

	// put new path into user_data
	return path_copy(temp_path, user_data->dir, PATH_MAX);
}

static int putchar(int ch) {
	return putc((char) ch);
}

static uint32_t timer_read(void) {
	return timerget();
}

static void *malloc(size_t count) {
	return rm_alloc(user_data->malloc_context, count);
}

static void free(void *ptr) {
	rm_free(user_data->malloc_context, ptr);
}

static void *fopen(const char *path, int flags) {
	int fd = -1;

	// find the available fd
	for (int possible_fd = 0; possible_fd < PROGRAM_MAX_FILES; possible_fd++) {
		if (user_data->files[possible_fd].opened == false) {
			fd = possible_fd;
		}
	}

	if (fd == -1) {
		errno = EMFILE;
		return NULL;
	}

	if (load_path(path) < 0) {
		return NULL;
	}

	printf("temp_path = '%s'\n", temp_path);

	if (fileopen(&user_data->files[fd], temp_path, flags)) {
		return &user_data->files[fd];
	} else {
		return NULL;
	}
}

static int fclose(void *fp) {
	return fileclose(fp) ? 0 : -1;
} 

static size_t fread(void *buffer, size_t size, size_t count, void *fp) {
	int read_result = fileread(fp, buffer, size * count);

	if (read_result > 0) {
		return read_result / size;
	} else {
		return 0;
	}
}

static size_t fwrite(const void *buffer, size_t size, size_t count, void *fp) {
	int read_result = filewrite(fp, buffer, size * count);

	if (read_result > 0) {
		return read_result / size;
	} else {
		return 0;
	}
}

static long ftell(void *fp) {
	if (fp) {
		return filetell(fp);
	} else {
		return -1;
	}
}

static int fseek(void *fp, uint32_t offset) {
	return fileseek(fp, offset) ? 0 : -1;
}

static int fflush(void *fp) {
	return fileflush(fp) ? 0 : -1;
}

static void frewind(void *fp) {
	filereset(fp);
}

static int fresize(void *fp, uint32_t size) {
	return fileresize(fp, size) ? 0 : -1;
}

static uint32_t fsize(void *fp) {
	return filesize(fp);
}

// TODO: add more system functions
// TODO: timer_wait[_until]
// TODO: memset, memcpy, etc. (and other stdlib functions)
#define SYS_FUNC_LIST \
	X(getcwd) \
	X(setcwd) \
	X(printf) \
	X(puts) \
	X(putchar) \
	X(timer_read) \
	X(fopen) \
	X(fread) \
	X(fwrite) \
	X(fclose) \
	X(fsize) \
	X(fresize) \
	X(fseek) \
	X(fflush) \
	X(ftell) \
	X(frewind) \
	X(malloc) \
	X(free)

struct user_function_def __user_functions[] = {
#define X(N) {#N, (uintptr_t) N},
	SYS_FUNC_LIST
#undef X
};

ssize_t user_function_index(const char *name) {
	const size_t len = sizeof(__user_functions) / sizeof(*__user_functions);

	for (size_t i = 0; i < len; i++) {
		if (strncmp(name, __user_functions[i].name, SYSDEF_MAX_LEN) == 0)
			return i;
	}

	return -1;
}

uintptr_t user_function_ptr(const char *name) {
	ssize_t index = user_function_index(name);

	return index == -1 ? 0 : (uintptr_t) __user_functions[index].func;
}

void user_function_data_block(struct program_data *data) {
	user_data = data;
}
