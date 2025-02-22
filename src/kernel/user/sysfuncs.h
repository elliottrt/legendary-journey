#ifndef _SYSFUNCS
#define _SYSFUNCS

#include "common/types.h"

#define SYSDEF_MAX_LEN (16)

struct sysdef {
	char name[SYSDEF_MAX_LEN];
	uint32_t func;
};

uint32_t sysfunc_get(const char *name);

bool sysfunc_exists(const char *name);

#endif
