#include "std/std.h"

int main(int argc, const char **argv) {

	if (argc != 2) {
		printf("usage: %s <filename>\n", argv[0]);
		return 1;
	}

	struct file file;
	if (!fileopen(&file, argv[1], FCREATE | FDIRECTORY)) {
		printf("error: unable to create %s: %m\n", argv[1]);
		return 1;
	}

	fileclose(&file);
	
	return 0;
}
