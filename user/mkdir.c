#include "std/std.h"

int main(int argc, const char **argv) {

	if (argc < 2) {
		printf("usage: %s <filename> [<filename> ...]\n", argv[0]);
		return 1;
	}

	struct file file;

	for (int i = 1; i < argc; i++) {
		if (!fileopen(&file, argv[i], FCREATE | FDIRECTORY)) {
			printf("error: unable to create %s: %m\n", argv[i]);
			return 1;
		}
		fileclose(&file);
	}
	
	return 0;
}
