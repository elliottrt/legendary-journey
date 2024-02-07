#ifndef _FILE
#define _FILE

#include "fat.h"
#include "types.h"

#define PATH_SEP '/'

// TODO: strerror(int errno)
enum ferror {
	ENOENT = 0, /* no such file or directory */
	EBADF = 1, /* bad file descriptor */
	ENOTDIR = 2, /* tried to read something not a directory as a directory */
	EISDIR = 3, /* is a directory */
	EINVAL = 4, /* invalid argument */
	EPATH = 5, /* bad file path */
	EEXIST = 6, /* file already exists */
	EACCES = 7, /* permission denied */
	EIO = 8, /* general io error */
	ENAMETOOLONG = 9, /* filename too long */
	ENOMEM = 10, /* not enough space / can't allocate memory */
	ENOSYS = 11, /* function not implemented */
	ENOTEMPTY = 12, /* directory not empty */
	EPERM = 13, /* operation not permitted */
};

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
// TODO: fileremove(const char *pathname);
// TODO: fileremove(struct file *file); <- is this something I want to do?

extern int errno;

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
