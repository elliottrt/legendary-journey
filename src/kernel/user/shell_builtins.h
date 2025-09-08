#ifndef _KERNEL_SHELL_BUILTINS
#define _KERNEL_SHELL_BUILTINS

#include "kernel/user/shell.h"

// TODO: clear screen builtin

typedef int (*shell_builtin_t)(int, char **, struct shell_state *);

// checks if a given function is builtin. if it is,
// returns its pointer, otherwise null
shell_builtin_t shell_check_builtin(const char *name);

int shell_builtin_cd(int argc, char **argv, struct shell_state *state);

#endif
