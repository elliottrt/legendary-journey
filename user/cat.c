#include <stdlib.h>

int main(int argc, const char **argv) {

	if (argc != 2) {
		printf("usage: cat <filename>\n");
		return 1;
	}

	FILE *file = NULL;
	if ((file = fopen(argv[1], 0)) == NULL) {
		printf("error: unable to open %s: %m\n", argv[1]);
		return 1;
	}

	char buffer[32];
	size_t buf_size = 0;

	while ((buf_size = fread(buffer, 1, sizeof(buffer), file)) > 0) {
		for (size_t i = 0; i < buf_size; i++)
			putchar(buffer[i]);
	}

	fclose(file);
	
	return 0;
}
