#include "shell.h"

#include "elf.h"
#include "user/program.h"
#include "std.h"
#include "graphics/printf.h"
#include "drivers/kbd.h"

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

		if (argc == SHELL_MAX_ARGS)
			return SHELL_FAIL;

		// copy arg into argv
		argv[argc++] = cmd_iter;

		// if more args left, set cmd_iter to start of next
		if (end) cmd_iter = end + 1;
		// otherwise do this to end loop
		else cmd_iter = NULL;
	}

	// make sure at least 1 arg
	if (argc < 1) return SHELL_FAIL;

	user_entry_t entry = program_load(argv[0]);
	return entry ? entry(argc, argv) : SHELL_FAIL;
}

int shell(void) {
	printf(SHELL_PROMPT);

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
			putchar(ch);
			break;
		} else if (ch == '\b') {
			if (cmd_idx > 0) {
				cmd[--cmd_idx] = 0;
				putchar(ch);
			}
		} else {
			cmd[cmd_idx++] = ch;
			putchar(ch);
		}
	}

	// reset errno and execute command
	errno = 0;
	int result = shell_exec(cmd);

	// if there's an error, display it
	if (result == SHELL_FAIL) {
		printf("shell: error: %s\n", strerror(errno));
	}

	return result;
}
