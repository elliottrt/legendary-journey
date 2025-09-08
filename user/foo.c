#include <stdlib.h>

int main(int argc, char** argv) {
	if (setcwd("folder") < 0) return 1;

	printf("getcwd() = %s\n", getcwd());

	FILE *f = fopen("thing.c", 0);

	char buffer[32];
	size_t buf_size = 0;

	while ((buf_size = fread(buffer, 1, sizeof(buffer), f)) > 0) {
		for (size_t i = 0; i < buf_size; i++)
			putchar(buffer[i]);
	}

	fclose(f);

	return 0;
}
