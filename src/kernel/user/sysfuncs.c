#include "sysfuncs.h"
#include "std.h"

#include "graphics/printf.h"
#include "drivers/timer.h"

uint64_t timer_read(void) {
	return timerget();
}

// TODO: add more system functions
struct sysdef __sysdefs[] = {
	{"printf", 	(uint32_t) printf},
	{"puts",   	(uint32_t) puts},
	{"putchar",	(uint32_t) putc},
	{"timer",  	(uint32_t) timer_read}
};

uint32_t sysfunc_get(const char *name) {
	size_t len = sizeof(__sysdefs) / sizeof(*__sysdefs);

	for (size_t i = 0; i < len; i++) {
		if (strncmp(name, __sysdefs[i].name, SYSDEF_MAX_LEN) == 0)
			return __sysdefs[i].func;
	}

	return 0;
}
