#include "std/std.h"

int main(int argc, const char **argv) {

	if (argc < 2) {
		printf("usage: %s <filename> [<filename> ...]\n", argv[0]);
		return 1;
	}

	FILE *file = NULL;
	for (int i = 1; i < argc; i++) {
		if ((file = fopen(argv[i], FCREATE)) == NULL) {
			printf("error: unable to create %s: %m\n", argv[i]);
			return 1;
		}
		fclose(file);
	}
	
	return 0;
}
