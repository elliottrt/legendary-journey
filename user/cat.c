#include "std/std.h"

int main(int argc, const char **argv) {

	if (argc != 2) {
		printf("usage: cat <filename>\n");
		return 1;
	}

	struct file file;
	if (!fileopen(&file, argv[1], 0))
		return -1;

	char buffer[32];
	size_t buf_size = 0;

	while ((buf_size = fileread(&file, buffer, sizeof(buffer))) > 0) {
		for (size_t i = 0; i < buf_size; i++)
			putc(buffer[i]);
	}

	fileclose(&file);
	
	return 0;
}
