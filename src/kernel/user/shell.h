#ifndef _SHELL
#define _SHELL

#define SHELL_FAIL (-1)
#define SHELL_MAX_ARGS (16) // maximum function arguments
#define SHELL_MAX_CMD_LEN (256) // maximum size of a shell command
#define SHELL_PROMPT "$ "

// TODO: shell_exec shouldn't modify the command string

// returns either the result of the command on success
// or SHELL_FAIL on failure
// note: command MUST be in modifiable memory or a pagefault will occur
int shell_exec(char *command);

// run the shell - gets input then executes input
int shell(void);

#endif
