#include "kernel/user/user_functions.h"
#include "common/std.h"

#include "kernel/graphics/printf.h"
#include "kernel/drivers/timer.h"
#include "common/file.h"

uint64_t timer_read(void) {
	return timerget();
}

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
	X(filetell)

struct sysdef __sysdefs[] = {
#define X(N) {#N, (uint32_t) N},
	SYS_FUNC_LIST
#undef X
};

ssize_t sysfunc_index(const char *name) {
	const size_t len = sizeof(__sysdefs) / sizeof(*__sysdefs);

	for (size_t i = 0; i < len; i++) {
		if (strncmp(name, __sysdefs[i].name, SYSDEF_MAX_LEN) == 0)
			return __sysdefs[i].func;
	}

	return -1;
}

uint32_t sysfunc_get(const char *name) {
	ssize_t index = sysfunc_index(name);

	return index == -1 ? 0 : (uint32_t) index;
}

bool sysfunc_exists(const char *name) {
	return sysfunc_index(name) != -1;
}
