#include "shell.h"

#include "elf.h"
#include "user/program.h"
#include "std.h"
#include "graphics/printf.h"

int shell_exec(char *command) {
	char *argv[SHELL_MAX_ARGS] = {0};
	int argc = 0;

	// trim starting spaces
	while (*command == ' ') command++;
	if (*command == '\0') return SHELL_FAIL;

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

	for (int i = 0; i < argc; i++) {
		printf("arg %d: %s\n", i, argv[i]);
	}

	// make sure at least 1 arg
	if (argc < 1) return SHELL_FAIL;

	user_entry_t entry = program_load(argv[0]);
	return entry ? entry(argc, argv) : SHELL_FAIL;
}
