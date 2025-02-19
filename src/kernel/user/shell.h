#ifndef _SHELL
#define _SHELL

#define SHELL_FAIL (-1)
#define SHELL_MAX_ARGS (16) // maximum function arguments

// returns either the result of the command on success
// or SHELL_FAIL on failure
// note: command MUST be in modifiable memory or a pagefault will occur
int shell_exec(char *command);

#endif
