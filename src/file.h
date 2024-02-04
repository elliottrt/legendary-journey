#ifndef _FILE
#define _FILE

#include "fat.h"
#include "types.h"

#define PATH_SEP '/'

enum ferror {
	ENOENT = 0, /* no such file or directory */
	EBADF = 1, /* bad file descriptor */
	ENOTDIR = 2, /* tried to read something not a directory as a directory */
	EINVAL = 3, /* invalid argument */
	EPATH = 4, /* bad file path */
	EEXIST = 5, /* file already exists */
};

// TODO: store fatdirentry inside of file struct
struct file {
	uchar filename[FAT_FILETOTAL_LEN + 1];

	uchar isdirectory;
	uchar opened;
	uchar dirty;
	uchar attributes;

	uint position;
	uint size;
	uchar buffer[SECTOR_SIZE];

	uint totalclusters;
	uint firstcluster;
	uint lastcluster;
    uint currentcluster;
	uint parentdircluster;
    uint sectorincluster;

};

enum fflags {
	FTRUNC = BIT(0), /* open file at start */
	FAPPEND = BIT(1), /* open file at end */
	FCREATE = BIT(2), /* create a file if none exists */
	FDIRECTORY = BIT(3), /* create a dir if FCREATE set */
};	

// TODO: function for editing entry in directory

// TODO: flag in direntry: is file open already
// if so, user shouldn't be able to edit/rename/delete
// etc. that file

// TODO: filecommit(struct file *file)

// TODO: remove FDIRECTORY and create separate function

// TODO: make fileexpand into fileresize, allow shrinking

extern int errno;

void fileinit(void);

int fileopen(struct file *file, const char *pathname, int flags);
int fileread(struct file *file, void *buffer, uint size);
int filewrite(struct file *file, const void *buffer, uint size);
int filereset(struct file *file);
int fileseek(struct file *file, uint seek);
int fileclose(struct file *file);

#endif
