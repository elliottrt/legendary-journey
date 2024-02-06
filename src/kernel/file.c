#include "file.h"
#include "std.h"
#include "printf.h"

// UNUSED
int filewrite(struct file *file, const void *buffer, uint size);

// TODO: move errno to a stdlib file
int errno = -1;

enum staticfileid {
	STATIC_FCLOSE = 0,
	STATIC_FCREAT0 = 1,
	STATIC_FCREAT1 = 2,
	STATIC_SIZE = 3
};

// TODO: dynamically allocate this in fileinit
static struct file _staticfiles[STATIC_SIZE];

int _fileread(struct file *file, void *buffer, uint size);
int _filewrite(struct file *file, const void *buffer, uint size);

void filenew(uint parentcluster, struct fatdirentry *entry, struct file *fileout) {
	memcpy(&fileout->fsentry, entry, sizeof(struct fatdirentry));
	fileout->opened = 1;
	fileout->dirty = 0;
	fileout->position = 0;
	fileout->firstcluster = entry->firstclusterlo | (uint) entry->firstclusterhi << 16;
	fileout->totalclusters = fattotalclusters(fileout->firstcluster, &fileout->lastcluster);
	fileout->currentcluster = fileout->firstcluster;
    fileout->sectorincluster = 0;
	fileout->parentdircluster = parentcluster;

	if (entry->attributes & DIRECTORY)
		fileout->fsentry.filesize = fileout->totalclusters * _vbootsector->sectorspercluster * _vbootsector->bytespersector;

	fatreadsector(fileout->firstcluster, 0, fileout->buffer);
}

int fileexpand(struct file *file, uint size) {

	if (file == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (file->opened == 0) {
		errno = EBADF;
		return -1;
	}

	uint bytespercluster = _vbootsector->sectorspercluster * _vbootsector->bytespersector;

	uint currentcapacity = file->totalclusters * bytespercluster;

	if (size > currentcapacity) {

		// new clusters (rounded up)
		uint clusterstoalloc = (size - currentcapacity + bytespercluster - 1) / bytespercluster;

		file->totalclusters += clusterstoalloc;
		file->lastcluster = fatallocclusters(file->lastcluster, clusterstoalloc);

	}

	fatcommit();

	return 0;

}

// returns entry index on success
int dirfindentry(struct file *dir, const char *name, struct fatdirentry *entry) {

	// TODO: is directory macro
	if (!(dir->fsentry.attributes & DIRECTORY)) {
		errno = ENOTDIR;
		return -1;
	}

	// TODO: this should be the same as dir->size, check that
	int possibleentries = (dir->totalclusters * _vbootsector->sectorspercluster * _vbootsector->bytespersector) 
								/ sizeof(struct fatdirentry);

	uint entryindex = 0;

	if (dir->position != 0)
		filereset(dir);

	printf("debug looking for %s\n", name);

	do {

		if (_fileread(dir, entry, sizeof(struct fatdirentry)) < 0) 
			return -1;

		printf("debug %u: %s\n", entryindex, entry->filename);

		if (strncmp((char *) entry->filename, name, FAT_FILETOTAL_LEN) == 0)
			return entryindex;

		entryindex++;

	} while(fatnomoredirentry(entry) == 0 && possibleentries--);

	errno = ENOENT;
	return -1;

}

int fromentry(struct file *directory, const char *name, struct file *fileout) {

	struct fatdirentry entry;

	if (!(directory->fsentry.attributes & DIRECTORY)) {
		errno = ENOTDIR;
		return -1;
	}

	// find entry
	if (dirfindentry(directory, name, &entry) < 0)
		return -1;

	filenew(directory->firstcluster, &entry, fileout);

	return 0;
}

int diraddentry(struct file *dir, struct fatdirentry *entry) {

	if (dir == NULL || entry == NULL) {
		errno = EINVAL;
		return -1;
	}

	printf("debug stop 1.5?\n");

	if (dir->opened == 0) {
		errno = EBADF;
		return -1;
	}

	if (!(dir->fsentry.attributes & DIRECTORY)) {
		errno = ENOTDIR;
		return -1;
	}

	struct fatdirentry unused = {0};
	int entryindex = dirfindentry(dir, (char *) entry->filename, &unused);

	// if entry already exists
	if (entryindex >= 0) {
		errno = EEXIST;
		return -1;
	// if other error happened
	} else if (errno != ENOENT) {
		return -1;
	}

	// if we're here, the entry doesn't exist
	// and no other error happened.
	// file->position is sizeof(struct fatdirentry) bytes past
	// where we want to write
	// TODO: update this when we implement deleting files

	dir->dirty = 1;
	printf("debug stop 1.75, size = %d, seek = %d\n", dir->fsentry.filesize, dir->position - sizeof(struct fatdirentry));
	return fileseek(dir, dir->position - sizeof(struct fatdirentry)) + _filewrite(dir, entry, sizeof(struct fatdirentry));

}

int dircreatefile(struct file *dir, struct file *fileout, const char *filename, int isdirectory) {

	if (dir == NULL || filename == NULL) {
		errno = EINVAL;
		return -1;
	}

	printf("debug zero stop?\n");

	if (dir->opened == 0) {
		errno = EBADF;
		return -1;
	}

	if (!(dir->fsentry.attributes & DIRECTORY)) {
		errno = ENOTDIR;
		return -1;
	}

	struct fatdirentry newentry = {0};

	if (isdirectory)
		newentry.attributes |= DIRECTORY;
	fatformatfilename(filename, strlen(filename), (char *) newentry.filename);

	uint filefirstcluster = fatfindfreecluster();
	if (filefirstcluster == 0)
		return -1;
	fatsetcluster(filefirstcluster, FEEND32L);
	newentry.firstclusterlo = filefirstcluster & 0xFFFF;
	newentry.firstclusterhi = filefirstcluster >> 16;

	// write direntry to dir
	if (diraddentry(dir, &newentry) < 0)
		return -1;

	printf("debug first stop?\n");

	// write . and .. entries
	if (isdirectory) {

		// open child dir
		filenew(dir->firstcluster, &newentry, &_staticfiles[STATIC_FCREAT1]);

		// TODO: change access/modify dates
		struct fatdirentry dot = {0};
		fatformatfilename(FAT_DOT, FAT_FILETOTAL_LEN, (char *) dot.filename);
		dot.firstclusterlo = newentry.firstclusterlo;
		dot.firstclusterhi = newentry.firstclusterhi;

		struct fatdirentry dotdot = {0};
		fatformatfilename(FAT_DOTDOT, FAT_FILETOTAL_LEN, (char *) dotdot.filename);
		dotdot.firstclusterlo = dir->firstcluster & 0xFFFF;
		dotdot.firstclusterhi = dir->firstcluster >> 16;

		if (diraddentry(&_staticfiles[STATIC_FCREAT1], &dot) < 0 || 
			diraddentry(&_staticfiles[STATIC_FCREAT1], &dotdot) < 0)
			return -1;

		// since we have exactly two entries we are guaranteed to be
		// in the first cluster
		fatwritesector(_staticfiles[STATIC_FCREAT1].firstcluster, 0, _staticfiles[STATIC_FCREAT1].buffer);
	}

	if (fileout) {
		filenew(dir->firstcluster, &newentry, fileout);
		fileout->dirty = 1;
	}

	fatcommit();

	dir->dirty = 1;
	return 0;

}

uint pathnext(char **start) {
	char *cursor = *start;
	uint size = 0;
	// move past current path
	while (*cursor != PATH_SEP && *cursor) {
		(*start)++;
		cursor++;
	}
	// ignore multiple path sep
	while (*cursor == PATH_SEP) {	
		(*start)++;
		cursor++;
	}
	// *start is where is should be, continue to
	// find the size of the path
	while ( *cursor != PATH_SEP && *cursor) {
		size++;
		cursor++;
	}
	return size;
}


void fileinit(void) {
	/* intentionally left empty */
}

int fileopen(struct file *file, const char *pathname, int flags) {

	if (flags == 0)
		flags = FTRUNC;

	// open root in provided pointer
	struct fatdirentry rootentry = FAT_VIRT_ROOT_ENTRY;
	filenew(0, &rootentry, file);

	int pathnextsize = 0;

	char *start = (char *) pathname;
	char fatformattedname[FAT_FILETOTAL_LEN + 1];

	if (pathname == NULL || pathname[0] != PATH_SEP) {
		errno = EPATH;
		return -1;
	}

	// TODO: test this with increased folder depth
	while ((pathnextsize = pathnext(&start)) > 0) {

		fatformatfilename(start, pathnextsize, fatformattedname);

		printf("debug path left: %s (%u bytes)\n", start, pathnextsize);
		printf("debug path next: %s\n", fatformattedname);

		char *tempstart = start;

		if (fromentry(file, fatformattedname, file) < 0) {

			// if we don't find the next step and this is the last
			// part of the filepath, create a new file (if that's)
			// what's desired or return an error
			char *filename = start;

			// create file
			if ((flags & FCREATE) && pathnext(&tempstart) == 0) {
				memcpy(&_staticfiles[STATIC_FCREAT0], file, sizeof(struct file));
				printf("debug creating file\n");
				int res = dircreatefile(&_staticfiles[STATIC_FCREAT0], file, filename, flags & FDIRECTORY);
				fileclose(&_staticfiles[STATIC_FCREAT0]);
				if (res < 0) memset(file, 0, sizeof(struct file));
				return res;
			}

			memset(file, 0, sizeof(struct file));
			errno = ENOENT;
			return -1;
		
		} else if ((flags & FCREATE) && pathnext(&tempstart) == 0) {
			// if we find the last part of the filepath and it's the same name
			// and we're trying to create a file, we error
			memset(file, 0, sizeof(struct file));
			errno = EEXIST;
			return -1;
		}

	}

	file->opened = 1;

	if (flags & FAPPEND)
		fileseek(file, file->fsentry.filesize);

	return 0;

}

int _fileread(struct file *file, void *buffer, uint size)
{
	uint sectorpos, newsectorstoread;
	uchar *bytebuffer = (uchar *) buffer;
	uint filetotalbytes = file->totalclusters * _vbootsector->sectorspercluster * _vbootsector->bytespersector;

	if (!(file->fsentry.attributes & DIRECTORY))
		filetotalbytes = min(filetotalbytes, file->fsentry.filesize);

	size = min(size, filetotalbytes - file->position);

	sectorpos = file->position % SECTOR_SIZE;
	newsectorstoread = (sectorpos + size) / SECTOR_SIZE;

	while (newsectorstoread--)
	{
		memcpy(bytebuffer, &file->buffer[sectorpos], SECTOR_SIZE - sectorpos);
		file->position += SECTOR_SIZE - sectorpos;
		bytebuffer += SECTOR_SIZE - sectorpos;
		sectorpos = 0;

		file->sectorincluster++;
		if (file->sectorincluster >= _vbootsector->sectorspercluster)
		{
			file->sectorincluster = 0;
			file->currentcluster = fatclustervalue(file->currentcluster);
			if (file->currentcluster >= FEEND32L)
			{
				/* reached eof */
				file->position = file->fsentry.filesize;
				return 0;
			}
		}

		fatreadsector(file->currentcluster, file->sectorincluster, file->buffer);

	}

	// TODO: this seems wrong, we never seem to adjust size
	// so we might be reading way too much data
	memcpy(bytebuffer, &file->buffer[sectorpos], size);
	file->position += size;
	return size;
}

int fileread(struct file *file, void *buffer, uint size) {
	if (file == NULL || buffer == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	if (file->opened == 0)
	{
		errno = EBADF;
		return -1;
	}

	if (size == 0) return 0;

	return _fileread(file, buffer, size);
}

// TODO: fileexpand(file *file, uint newbytes) function from the code in here
int _filewrite(struct file *file, const void *buffer, uint size) {
	const uchar *bytebuffer = (const uchar *) buffer;

	uint maxpossiblebytes = file->position + size;

	file->dirty = 1;

	// increase allocated file disk space if necessary
	fileexpand(file, maxpossiblebytes);

	uint bytesleft = size;
	uint sectorpos = file->position % SECTOR_SIZE;
	int newsectorstowrite = (sectorpos + size) / SECTOR_SIZE;

	// write to buffer
	do {
		uint bytestowrite = min(bytesleft, SECTOR_SIZE - sectorpos);

		memcpy(file->buffer + sectorpos, bytebuffer, bytestowrite);
		file->position += bytestowrite;
		bytebuffer += bytestowrite;
		bytesleft  -= bytestowrite;
		sectorpos = 0;

		if (bytesleft > 0) {
			fatwritesector(file->currentcluster, file->sectorincluster, file->buffer);

			file->sectorincluster++;
			if (file->sectorincluster >= _vbootsector->sectorspercluster) {
				file->sectorincluster = 0;
				file->currentcluster = fatclustervalue(file->currentcluster);
				if (file->currentcluster >= FEEND32L) {
					printferr();
					printf("reached eof while writing to file, this should never happen\n");
					printfstd();
				}
			}
		}
	} while ((newsectorstowrite--) > 0);

	// update file size
	// if directory, size == clusters * bytes/cluster
	// else file size == sizeof contents
	file->fsentry.filesize = (file->fsentry.attributes & DIRECTORY) ? file->totalclusters * _vbootsector->sectorspercluster * _vbootsector->bytespersector : umax(file->fsentry.filesize, file->position);

	return 0;
}

int filewrite(struct file *file, const void *buffer, uint size) {
	if (file == NULL || buffer == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (file->opened == 0){
		errno = EBADF;
		return -1;
	}

	if (size == 0) return 0;

	return _filewrite(file, buffer, size);
}

int filereset(struct file *file)
{

	if (file == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	if (file->opened == 0)
	{
		errno = EBADF;
		return -1;
	}

	/* read new sector if the current buffer isn't the very first sector */
	if (file->position >= SECTOR_SIZE)
		fatreadsector(file->firstcluster, 0, file->buffer);

	file->position = 0;
    file->currentcluster = file->firstcluster;
    file->sectorincluster = 0;

    return 0;

}

int fileseek(struct file *file, uint seek) {

	if (file == NULL || seek > file->fsentry.filesize) {
		errno = EINVAL;
		return -1;
	}

	if (file->opened == 0) {
		errno = EBADF;
		return -1;
	}

	uint clustersin = seek / (_vbootsector->sectorspercluster * _vbootsector->bytespersector);
	uint newsector = (seek / _vbootsector->bytespersector) % _vbootsector->sectorspercluster;

	// get the cluster our target is in
	uint newcluster = file->firstcluster;
	while (clustersin--) 
		newcluster = fatclustervalue(newcluster);

	if (file->currentcluster != newcluster || file->sectorincluster != newsector)
		fatreadsector(newcluster, newsector, file->buffer);

	file->position = seek;
	file->currentcluster = newcluster;
	file->sectorincluster = newsector;

	return 0;
}

int fileclose(struct file *file) {

	if (file == NULL)  {
		errno = EINVAL;
		return -1;
	}

	if (file->opened == 0) {
		errno = EBADF;
		return -1;
	}

	printf("debug closing %s, size=%d\n", file->fsentry.filename, file->fsentry.filesize);

	// TODO: better way to edit files
	if (file->dirty) {

		// update size in parent dir
		if (file->parentdircluster != 0) {

			struct fatdirentry parentfakeentry = { 
				.attributes = DIRECTORY, 
				.firstclusterlo = file->parentdircluster, 
				.firstclusterhi = file->parentdircluster >> 16
			};
			filenew(0, &parentfakeentry, &_staticfiles[STATIC_FCLOSE]);
			_staticfiles[STATIC_FCLOSE].opened = 1;

			printf("debug name=%s\n", file->fsentry.filename);

			// parentfakeentry used here as dummy
			if (dirfindentry(&_staticfiles[STATIC_FCLOSE], (char *) file->fsentry.filename, &parentfakeentry) < 0)
				return -1;
			// TODO: set accesstime and modifydate and modifytime

			printf("debug get here 1\n");

			uint entryposition = _staticfiles[STATIC_FCLOSE].position - sizeof(struct fatdirentry);

			// goto entry position
			if (fileseek(&_staticfiles[STATIC_FCLOSE], entryposition) < 0)
				return -1;

			// write entry to file
			if (_filewrite(&_staticfiles[STATIC_FCLOSE], &file->fsentry, sizeof(struct fatdirentry)) < 0)
				return -1;

			// flush parent dir buffer to disk
			if (fatwritesector(_staticfiles[STATIC_FCLOSE].currentcluster, 
							   _staticfiles[STATIC_FCLOSE].sectorincluster, 
							   _staticfiles[STATIC_FCLOSE].buffer) < 0)
				return -1;

			// close parent dir
			_staticfiles[STATIC_FCLOSE].opened = 0;

		}

		// write buffer and fat table to disk
		fatwritesector(file->currentcluster, file->sectorincluster, file->buffer);
		fatcommit();

	}

	file->dirty = 0;
	file->opened = 0;

	return 0;
}
