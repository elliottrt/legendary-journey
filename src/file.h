#ifndef _FILE
#define _FILE

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
	uchar isdirectory;
	uchar opened;
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

void fileinit(void);

int fileopen(struct file *file, const char *pathname);
int fileread(struct file *file, void *buffer, uint size);
int filereset(struct file *file);
int fileseek(struct file *file, uint seek);

#endif
