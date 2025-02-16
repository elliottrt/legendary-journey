#ifndef _FILE
#define _FILE

#include "fat.h"
#include "types.h"

#define PATH_SEP '/'

#define fileisdir(fp) ((fp->fsentry.attributes & DIRECTORY) != 0)
#define entryisdir(ep) ((ep->attributes) != 0)

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
	FDIRECTORY = BIT(3), /* create a dir if FCREATE set */
};

// TODO: flag in direntry: is file open already
// if so, user shouldn't be able to edit/rename/delete
// etc. that file

// TODO: remove FDIRECTORY and create separate function
// TODO: fileremove(struct file *parent, const char *pathname); <- parent can be null

void fileinit(void);

// TODO: definitions in kernel's file.c don't match... also I should probably write documentations

bool fileresize(struct file *file, uint32_t size);
bool fileopen(struct file *file, const char *pathname, int flags);
// returns amount of bytes read
int32_t fileread(struct file *file, void *buffer, uint32_t size);
// returns whether operation completed successfully
bool filewrite(struct file *file, const void *buffer, uint32_t size);
bool filereset(struct file *file);
bool fileseek(struct file *file, uint32_t seek);
bool fileflush(struct file *file);
bool fileclose(struct file *file);

#endif
