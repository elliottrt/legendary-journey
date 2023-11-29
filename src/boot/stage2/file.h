#ifndef _STAGE2_FILE
#define _STAGE2_FILE

#include "fat.h"
#include "types.h"

#define PATH_SEP '/'

enum fileerror
{
	ENOENT = 0, /* no such file or directory */
	EBADF = 1, /* bad file descriptor */
	ENOTDIR = 2, /* tried to read something not a directory as a directory */
	EINVAL = 3, /* invalid argument */
	EPATH = 5 /* bad file path */
};

struct file
{
	short isdirectory;
	short opened;
	uint position;
	uint size;
	uint totalclusters;
	uint firstcluster;
    uint currentcluster;
    uint sectorincluster;
    uchar buffer[SECTOR_SIZE];
    uchar attributes;
};

extern int errno;
extern const char *strerror[5];

void fileinit(void);

int fileopen(struct file *file, const char *pathname);
int fileread(struct file *file, void *buffer, uint size);
int filereset(struct file *file);
int fileseek(struct file *file, uint seek);

#endif
