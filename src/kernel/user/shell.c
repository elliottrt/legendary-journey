#include "shell.h"

#include "common/elf.h"
#include "common/std.h"
#include "kernel/user/program.h"
#include "kernel/graphics/printf.h"
#include "kernel/drivers/kbd.h"
#include "kernel/memory/malloc.h"
#include "kernel/user/user_functions.h"

// TODO: keep list of paths to search for executables like ls, mkdir, etc. and put all of the user executables into /bin
// TODO: stdin, stdout, stderr FILE* in stdlib that are checked for in user_functions.c. stderr = stdout, use kbd_getc for stdin

// shell state
char dir[PATH_MAX] = DEFAULT_PATH;

// shell builtin definitions

static int shell_cd_usage(int retval, const char *message) {

	printf("usage: cd <path>\n");

	if (message) printf("error: %s\n", message);

	return retval;
}

static int shell_cd(int argc, char **argv) {
	if (argc != 2) {
		return shell_cd_usage(1, "no argument provided");
	}

	if (path_load(dir, argv[1], dir, sizeof(dir)) < 0) {
		return shell_cd_usage(1, strerror(errno));
	} else {
		return 0;
	}
}

// builtin function list
typedef int (*shell_builtin_t)(int, char**);
struct {
	const char name[8];
	shell_builtin_t func;
} shell_builtins[] = {
	{"cd", shell_cd}
};

// checks if a given function is builtin. if it is,
// returns its pointer, otherwise null
shell_builtin_t shell_check_builtin(const char *name) {
	for (size_t i = 0; i < sizeof(shell_builtins) / sizeof(*shell_builtins); i++) {
		if (strcmp(name, shell_builtins[i].name) == 0)
			return shell_builtins[i].func;
	}
	return NULL;
}

int shell_exec(char *command) {
	char *argv[SHELL_MAX_ARGS] = {0};
	int argc = 0;

	// trim starting spaces
	while (*command == ' ') command++;
	if (*command == '\0') return SHELL_FAIL;

	// trim ending spaces
	uint32_t command_len = strlen(command);
	char *back_iter = command + command_len - 1;
	while (*back_iter == ' ') {
		*back_iter = '\0';
		back_iter--;
	}

	char target;
	for (char *cmd_iter = command; cmd_iter && *cmd_iter;) {
		switch (*cmd_iter) {
			case '\'':
			case '"': target = *cmd_iter; cmd_iter++; break;
			default: target = ' '; break;
		}

		char *end = strchr(cmd_iter, target);

		// fail if no closing quote
		if (!end && target != ' ') return SHELL_FAIL;
		else if (end) *end = '\0'; // set end to be \0, allowing for argv within command

		if (argc >= SHELL_MAX_ARGS) {
			errno = E2BIG;
			return SHELL_FAIL;
		}

		// copy arg into argv
		argv[argc++] = cmd_iter;

		// if more args left, set cmd_iter to start of next
		if (end) cmd_iter = end + 1;
		// otherwise do this to end loop
		else cmd_iter = NULL;
	}

	return shell_exec_args(argc, argv);
}

int shell_exec_args(int argc, char **argv) {
	// make sure at least 1 arg
	if (argc < 1) return SHELL_FAIL;

	shell_builtin_t maybe_builtin = shell_check_builtin(argv[0]);
	if (maybe_builtin != NULL) {
		return maybe_builtin(argc, argv);
	}

	struct program_data *data = user_mode_start(dir, argv[0]);
	if (data == NULL) return SHELL_FAIL;

	// set the context for user functions
	user_function_data_block(data);

	int retval = data->entry(argc, argv);
	user_mode_end(data);

	return retval;
}

int shell(void) {
	printf("%s%s", dir, SHELL_PROMPT);

	char cmd[SHELL_MAX_CMD_LEN] = {0};
	uint32_t cmd_idx = 0;

	while (1) {
		if (cmd_idx >= SHELL_MAX_CMD_LEN) {
			errno = ENOMEM;
			return SHELL_FAIL;
		}

		char ch = kbd_getc_blocking();

		if (ch == '\n') {
			// null terminate the command, replacing the new line
			// and finished getting input
			cmd[cmd_idx] = '\0';
			putc(ch);
			break;
		} else if (ch == '\b') {
			if (cmd_idx > 0) {
				cmd[--cmd_idx] = 0;
				putc(ch);
			}
		} else {
			cmd[cmd_idx++] = ch;
			putc(ch);
		}
	}

	// reset errno and execute command
	errno = 0;
	int result = shell_exec(cmd);

	// if there's an error, display it
	if (result == SHELL_FAIL) {
		printf("shell: error: %m\n");
	}

	return result;
}
