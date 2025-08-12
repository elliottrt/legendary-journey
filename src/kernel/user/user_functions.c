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

static uint64_t timer_read(void) {
	return timerget();
}

static void *malloc(size_t count) {
	return rm_alloc(user_data->malloc_context, count);
}

static void free(void *ptr) {
	rm_free(user_data->malloc_context, ptr);
}

// TODO: file io with either 'int fd' (posix) or 'FILE *p' (libc)

// TODO: add more system functions
#define SYS_FUNC_LIST \
	X(printf) \
	X(puts) \
	X(putc) \
	X(timer_read) \
	X(fileopen) \
	X(fileread) \
	X(fileclose) \
	X(filesize) \
	X(fileresize) \
	X(fileseek) \
	X(fileflush) \
	X(filetell) \
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
