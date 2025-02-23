#include "std/std.h"

int main(int argc, char **argv) {
	struct file dir;

	if (!fileopen(&dir, argc >= 2 ? argv[1] : "/", FDIRECTORY))
		return -1;

	struct fatdirentry child;

	while (fileread(&dir, &child, sizeof(child)) == sizeof(child)) {
		if (child.filename[0]) {
			for (int i = 0; i < FAT_FILENAME_LEN; i++)
				putc(child.filename[i]);

			if (child.fileext[0] && child.fileext[0] != ' ') putc('.');
			else putc(' ');

			for (int i = 0; i < FAT_FILEEXT_LEN; i++)
				putc(child.fileext[i]);

			if (child.attributes & DIRECTORY)
				printf("  (DIR)");
			
			putc('\n');
		}
	}

	fileclose(&dir);
	return 0;
}
