
#define FAT_FILENAME_LEN 8
#define FAT_FILEEXT_LEN 3
#define SECTOR_SIZE 512
#define FDIRECTORY (1 << 3)

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

typedef unsigned int bool;
#define true (1)
#define false (0)

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

struct file {
	
	struct fatdirentry fsentry;

	bool opened;
	bool dirty;

	uint32_t position;
	uint8_t buffer[SECTOR_SIZE];

	uint32_t totalclusters;
	uint32_t firstcluster;
	uint32_t lastcluster;
    uint32_t currentcluster;
	uint32_t parentdircluster;
    uint32_t sectorincluster;

};

extern bool fileopen(struct file *file, const char *pathname, int flags);
extern int32_t fileread(struct file *file, void *buffer, uint32_t size);
extern bool fileclose(struct file *file);

extern int printf(const char *format, ...);
extern int putchar(int ch);

int main(int argc, char **argv) {
	struct file dir;

	if (!fileopen(&dir, argc >= 2 ? argv[1] : "/", FDIRECTORY))
		return -1;

	struct fatdirentry child;

	while (fileread(&dir, &child, sizeof(child)) == sizeof(child)) {
		if (child.filename[0]) {
			for (int i = 0; i < FAT_FILENAME_LEN; i++)
				putchar(child.filename[i]);

			if (child.fileext[0] && child.fileext[0] != ' ') putchar('.');
			else putchar(' ');

			for (int i = 0; i < FAT_FILEEXT_LEN; i++)
				putchar(child.fileext[i]);

			if (child.attributes & 0x10)
				printf("  (DIR)");
			putchar('\n');
		}
	}

	fileclose(&dir);
	return 0;
}
