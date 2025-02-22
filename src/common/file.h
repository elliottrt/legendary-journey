#ifndef _FILE
#define _FILE

#include "fat.h"
#include "types.h"

#define PATH_SEP '/'

#define fileisdir(fp) ((fp->fsentry.attributes & DIRECTORY) != 0)
#define entryisdir(ep) ((ep)->attributes & DIRECTORY)

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

enum fflags {
	FTRUNC = BIT(0), /* open file at start */
	FAPPEND = BIT(1), /* open file at end */
	FCREATE = BIT(2), /* create a file if none exists */
	FDIRECTORY = BIT(3), /* open a dir, or create a dir if FCREATE set */
};

// TODO: flag in direntry: is file open already
// if so, user shouldn't be able to edit/rename/delete
// etc. that file

// TODO: fileremove(struct file *parent, const char *pathname); <- parent can be null
// TODO: filerename?

void fileinit(void);

// TODO: definitions in kernel's file.c don't match... also I should probably write documentations

bool fileresize(struct file *file, uint32_t size);
bool fileopen(struct file *file, const char *pathname, int flags);
// returns number of bytes read, or -1 on failure
int32_t fileread(struct file *file, void *buffer, uint32_t size);
// returns number of bytes written, or -1 on failure
int32_t filewrite(struct file *file, const void *buffer, uint32_t size);
bool filereset(struct file *file);
bool fileseek(struct file *file, uint32_t seek);
bool fileflush(struct file *file);
bool fileclose(struct file *file);
uint32_t filesize(struct file *file);
uint32_t filetell(struct file *file);

#endif
