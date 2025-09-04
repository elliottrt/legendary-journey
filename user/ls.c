#include "std/std.h"

#define FAT_FILENAME_LEN 8
#define FAT_FILEEXT_LEN 3

#define FAT_FLAG_ISDIR 0x10

struct fatdirentry {
	uint8_t filename[FAT_FILENAME_LEN];
	uint8_t fileext[FAT_FILEEXT_LEN];
	uint8_t attributes;
	uint8_t reserved;
	uint8_t creationtimetenths;
    uint16_t creationtime;
    uint16_t creationdate;
    uint16_t accessdate;
    uint16_t firstclusterhi;
    uint16_t modifytime;
    uint16_t modifydate;
    uint16_t firstclusterlo;
    uint32_t filesize;
} __attribute__ ((packed));

int main(int argc, char **argv) {
	FILE *dir = NULL;

	if ((dir = fopen(argc >= 2 ? argv[1] : "/", FDIRECTORY)) == NULL)
		return -1;

	struct fatdirentry child;

	while (fread(&child, sizeof(child), 1, dir) == 1) {
		if (child.filename[0]) {
			for (int i = 0; i < FAT_FILENAME_LEN; i++)
				putc(child.filename[i]);

			if (child.fileext[0] && child.fileext[0] != ' ') putc('.');
			else putc(' ');

			for (int i = 0; i < FAT_FILEEXT_LEN; i++)
				putc(child.fileext[i]);

			if (child.attributes & FAT_FLAG_ISDIR)
				printf("  (DIR)");
			
			putc('\n');
		}
	}

	fclose(dir);
	return 0;
}
