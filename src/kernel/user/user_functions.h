#ifndef _SYSFUNCS
#define _SYSFUNCS

#include "common/types.h"
#include "kernel/user/program.h"

uintptr_t user_function_ptr(const char *name);

bool user_function_exists(const char *name);

void user_function_data_block(struct program_data *data);

#endif
