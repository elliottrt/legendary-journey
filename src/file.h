#ifndef _FILE
#define _FILE

#include "fat.h"
#include "types.h"

#define PATH_SEP '/'

#define fileisdir(fp) ((fp->fsentry.attributes & DIRECTORY) != 0)
#define entryisdir(ep) ((ep->attributes) != 0)

struct file {
	
	struct fatdirentry fsentry;

	uchar opened;
	uchar dirty;

	uint position;
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

// TODO: flag in direntry: is file open already
// if so, user shouldn't be able to edit/rename/delete
// etc. that file

// TODO: remove FDIRECTORY and create separate function
// TODO: fileremove(struct file *parent, const char *pathname); <- parent can be null

void fileinit(void);

int fileresize(struct file *file, uint size);
int fileopen(struct file *file, const char *pathname, int flags);
int fileread(struct file *file, void *buffer, uint size);
int filewrite(struct file *file, const void *buffer, uint size);
int filereset(struct file *file);
int fileseek(struct file *file, uint seek);
int fileflush(struct file *file);
int fileclose(struct file *file);

#endif
