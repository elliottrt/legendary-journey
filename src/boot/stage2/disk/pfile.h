#ifndef _MKFAT_PFILE
#define _MKFAT_PFILE

#define MAXNAMELEN 255
#define MAXPATHLEN 1024
#define MAXOPENFILES 10

#define PATH_SEP '/'

#include <stdint.h>
#include "fat.h"

enum posixfileerror
{
	EPERM, /* operation not permitted */
	ENOENT, /* no such file or directory */
	EIO, /* I/O error */
	EBADF, /* bad file descriptor */
	EACCES, /* permission denied */
	EEXIST, /* file exists */
	ENOTDIR, /* tried to read something not a directory as a directory */
	EISDIR, /* tried to access directory as a file */ 
	EINVAL, /* invalid argument */
	ENFILE, /* too many open files */
	EFBIG, /* file too large */
	ENAMETOOLONG, /* filename too long */
	ENOTEMPTY, /* directory was not empty */
	ENOSYS, /* function not implemented */
	EFTYPE, /* inappropriate file type/format */
	EPATH /* file path didn't start with a '/' */
};

struct pfile
{
	int16_t is_directory;
	int16_t opened;
	uint32_t position;
	uint32_t size;
	uint32_t total_clusters;
	uint32_t first_cluster;
    uint32_t current_cluster;
    uint32_t sector_in_cluster;
    uint8_t buffer[SECTOR_SIZE];
    uint8_t attributes;
};

extern int32_t errno;

void pfile_init(void);

uint32_t path_next(char **start);

int32_t open(struct pfile *pfile, const char *pathname);
int32_t read(struct pfile *pfile, void *buffer, uint32_t size);
int32_t reset(struct pfile *pfile);

#endif
