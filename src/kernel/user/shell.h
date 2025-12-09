#ifndef _SHELL
#define _SHELL

#include "kernel/disk/path.h"

#define SHELL_FAIL (-1)
#define SHELL_MAX_ARGS (16) // maximum function arguments
#define SHELL_MAX_CMD_LEN (128) // maximum size of a shell command
#define SHELL_HISTORY_SIZE (16) // number of commands to remember
#define SHELL_PROMPT "$ "

// TODO: shell_exec shouldn't modify the command string

// state of the running shell,
// passed to builtin functions
struct shell_state {
	char dir[PATH_MAX];

	// pointer to allocated shell history list
	// the allocation will have SHELL_HISTORY_SIZE commands
	char (*history)[SHELL_MAX_CMD_LEN];
};

// returns either the result of the command on success
// or SHELL_FAIL on failure
// note: command MUST be in modifiable memory or a pagefault will occur
int shell_exec(char *command);

// returns either the result of the command on success
// or SHELL_FAIL on failure
int shell_exec_args(int argc, char **argv);

// run the shell - gets input then executes input
int shell(void);

#endif
