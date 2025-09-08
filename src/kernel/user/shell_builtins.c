#include "kernel/user/shell_builtins.h"
#include "kernel/graphics/printf.h"

static int shell_cd_usage(int retval, const char *message) {

	printf("usage: cd <path>\n");

	if (message) printf("error: %s\n", message);

	return retval;
}

int shell_builtin_cd(int argc, char **argv, struct shell_state *state) {
	if (argc != 2) {
		return shell_cd_usage(1, "no argument provided");
	}

	if (path_load(state->dir, argv[1], state->dir, sizeof(state->dir)) < 0) {
		return shell_cd_usage(1, strerror(errno));
	} else {
		return 0;
	}
}

struct {
	const char name[8];
	shell_builtin_t func;
} shell_builtins[] = {
	{"cd", shell_builtin_cd}
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
