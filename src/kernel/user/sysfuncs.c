#include "sysfuncs.h"
#include "std.h"

#include "graphics/printf.h"
#include "drivers/timer.h"
#include "file.h"

uint64_t timer_read(void) {
	return timerget();
}

// TODO: add more system functions
#define SYS_FUNC_LIST \
	X(printf) \
	X(puts) \
	X(putchar) \
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

uint32_t sysfunc_get(const char *name) {
	size_t len = sizeof(__sysdefs) / sizeof(*__sysdefs);

	for (size_t i = 0; i < len; i++) {
		if (strncmp(name, __sysdefs[i].name, SYSDEF_MAX_LEN) == 0)
			return __sysdefs[i].func;
	}

	return 0;
}
