#include "common/file.h"
#include "common/std.h"
#include "kernel/graphics/printf.h"

enum staticfileid {
	STATIC_FCREAT0 = 0,
	STATIC_FCREAT1 = 1,
	STATIC_SIZE = 2
};

struct file _staticfiles[STATIC_SIZE] = {0};

int32_t _fileread(struct file *file, void *buffer, uint32_t size);
int32_t _filewrite(struct file *file, const void *buffer, uint32_t size);

void filenew(uint32_t parentcluster, struct fatdirentry *entry, struct file *fileout) {
	memcpy(&fileout->fsentry, entry, sizeof(struct fatdirentry));
	fileout->opened = 1;
	fileout->dirty = 0;
	fileout->position = 0;
	fileout->firstcluster = entry->firstclusterlo | (uint32_t) entry->firstclusterhi << 16;
	fileout->totalclusters = fattotalclusters(fileout->firstcluster, &fileout->lastcluster);
	fileout->currentcluster = fileout->firstcluster;
    fileout->sectorincluster = 0;
	fileout->parentdircluster = parentcluster;

	if (entryisdir(entry))
		fileout->fsentry.filesize = fileout->totalclusters * _vbootsector->sectorspercluster * _vbootsector->bytespersector;

	// TODO: error check this?
	fatreadsector(fileout->firstcluster, 0, fileout->buffer);
}

bool fileresize(struct file *file, uint32_t size) {

	if (file == NULL) {
		errno = EINVAL;
		return false;
	}

	if (file->opened == 0) {
		errno = EBADF;
		return false;
	}

	uint32_t bytespercluster = _vbootsector->sectorspercluster * _vbootsector->bytespersector;
	uint32_t currentcapacity = file->totalclusters * bytespercluster;

	if (size > currentcapacity) {

		// new clusters (rounded up)
		uint32_t clusterstoalloc = (size - currentcapacity + bytespercluster - 1) / bytespercluster;

		file->totalclusters += clusterstoalloc;
		file->lastcluster = fatallocclusters(file->lastcluster, clusterstoalloc);

	} else if (size < currentcapacity) {

		// clusters to remove (rounded down)
		uint32_t clusterstoremove = (currentcapacity - size) / bytespercluster;

		file->totalclusters -= clusterstoremove;
		// removes clusters starting from the new last cluster
		file->lastcluster = fatdeallocclusters(fattraverse(file->firstcluster, file->totalclusters));
	}

	file->fsentry.filesize = size;
	file->dirty = true;

	fatcommit();

	return true;

}

// returns entry index on success
int32_t dirfindentry(struct file *dir, const char *name, struct fatdirentry *entry) {

	if (!fileisdir(dir)) {
		errno = ENOTDIR;
		return -1;
	}

	int possibleentries = (dir->totalclusters * _vbootsector->sectorspercluster * _vbootsector->bytespersector) 
								/ sizeof(struct fatdirentry);

	uint32_t entryindex = 0;

	// if user provided a null pointer, 
	// we use a dummy entry as storage
	struct fatdirentry dummyentry;
	if (entry == NULL) entry = &dummyentry;

	if (dir->position != 0)
		filereset(dir);

	do {

		if (_fileread(dir, entry, sizeof(struct fatdirentry)) != sizeof(struct fatdirentry)) 
			return -1;

		if (memcmp(entry->filename, name, FAT_FILETOTAL_LEN) == 0)
			return entryindex;

		entryindex++;

	} while(fatnomoredirentry(entry) == 0 && possibleentries--);

	errno = ENOENT;
	return -1;

}

bool dirsetentry(struct file *dir, uint32_t index, struct fatdirentry *entry) {

	uint32_t maxpossibleentries = (dir->totalclusters * _vbootsector->sectorspercluster * _vbootsector->bytespersector) / sizeof(struct fatdirentry);

	if (dir == NULL || entry == NULL || index >= maxpossibleentries) {
		errno = EINVAL;
		return false;
	}

	if (dir->opened == 0) {
		errno = EBADF;
		return false;
	}

	if (!fileisdir(dir)) {
		errno = ENOTDIR;
		return false;
	}

	// go to entry position
	if (fileseek(dir, index * sizeof(struct fatdirentry)) == 0)
		return false;

	// write entry there
	if (_filewrite(dir, entry, sizeof(struct fatdirentry)) < 0)
		return false;

	// update if needed
	return fileflush(dir);

}

int diraddentry(struct file *dir, struct fatdirentry *entry) {

	if (dir == NULL || entry == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (dir->opened == 0) {
		errno = EBADF;
		return -1;
	}

	if (!fileisdir(dir)) {
		errno = ENOTDIR;
		return -1;
	}

	int entryindex = dirfindentry(dir, (char *) entry->filename, NULL);

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

	if (!fileseek(dir, dir->position - sizeof(struct fatdirentry)))
		return -1;

	return _filewrite(dir, entry, sizeof(struct fatdirentry));

}

int dirupdateentry(struct file *dir, struct fatdirentry *entry) {

	if (dir == NULL || entry == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (dir->opened == 0) {
		errno = EBADF;
		return -1;
	}

	if (!fileisdir(dir)) {
		errno = ENOTDIR;
		return -1;
	}

	int index;

	// if it doesn't exist return that errorcode
	if ((index = dirfindentry(dir, (char *) entry->filename, NULL)) < 0)
		return -1;

	return dirsetentry(dir, index, entry);

}

int dircreatefile(struct file *dir, struct file *fileout, const char *filename, int isdirectory) {

	if (dir == NULL || filename == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (dir->opened == 0) {
		errno = EBADF;
		return -1;
	}

	if (!fileisdir(dir)) {
		errno = ENOTDIR;
		return -1;
	}

	struct fatdirentry newentry = {0};

	if (isdirectory) newentry.attributes |= DIRECTORY;
	fatformatfilename(filename, strlen(filename), (char *) newentry.filename);

	uint32_t filefirstcluster = fatfindfreecluster();
	if (filefirstcluster == 0)
		return -1;
	fatsetcluster(filefirstcluster, FEEND32L);
	newentry.firstclusterlo = filefirstcluster & 0xFFFF;
	newentry.firstclusterhi = filefirstcluster >> 16;

	// write direntry to dir
	if (diraddentry(dir, &newentry) < 0)
		return -1;

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

		fileflush(&_staticfiles[STATIC_FCREAT1]);
	}

	if (fileout) {
		filenew(dir->firstcluster, &newentry, fileout);
		fileout->dirty = 1;
	}

	fatcommit();

	dir->dirty = 1;
	return 0;

}

int filefromentry(struct file *directory, const char *name, struct file *fileout) {

	struct fatdirentry entry;

	if (!fileisdir(directory)) {
		errno = ENOTDIR;
		return -1;
	}

	// find entry
	if (dirfindentry(directory, name, &entry) < 0)
		return -1;

	filenew(directory->firstcluster, &entry, fileout);

	return 0;
}

uint32_t pathnext(char **start) {
	char *cursor = *start;
	uint32_t size = 0;
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

bool fileopen(struct file *file, const char *pathname, int flags) {

	if (flags == 0)
		flags = FTRUNC;

	// open root in provided pointer
	struct fatdirentry rootentry = FAT_VIRT_ROOT_ENTRY;
	filenew(0, &rootentry, file);

	int pathnextsize = 0;

	char *start = (char *) pathname;
	char fatformattedname[FAT_FILETOTAL_LEN];

	if (pathname == NULL || pathname[0] != PATH_SEP) {
		errno = EPATH;
		return false;
	}

	while ((pathnextsize = pathnext(&start)) > 0) {

		fatformatfilename(start, pathnextsize, fatformattedname);

		char *tempstart = start;

		if (filefromentry(file, fatformattedname, file) < 0) {

			// if we don't find the next step and this is the last
			// part of the filepath, either:
			// create a new file if FCREATE
			// or return an error
			char *filename = start;

			// create file
			if ((flags & FCREATE) && pathnext(&tempstart) == 0) {
				// TODO: try to not open a new file here
				memcpy(&_staticfiles[STATIC_FCREAT0], file, sizeof(struct file));
				int res = dircreatefile(&_staticfiles[STATIC_FCREAT0], file, filename, flags & FDIRECTORY);
				fileclose(&_staticfiles[STATIC_FCREAT0]);
				if (res < 0) {
					memset(file, 0, sizeof(struct file));
					errno = EIO;
				}
				return res >= 0;
			}

			memset(file, 0, sizeof(struct file));
			errno = ENOENT;
			return false;
		
		} else if ((flags & FCREATE) && pathnext(&tempstart) == 0) {
			// if we find the last part of the filepath and it's the same name
			// and we're trying to create a file, we error
			memset(file, 0, sizeof(struct file));
			errno = EEXIST;
			return false;
		}

		// TODO: this is a terrible fix. do something better
		// .. entries in folders in root have firstcluster = 0, so this patches that - terrible solution
		if (file->firstcluster == 0 && fileisdir(file)) {
			filenew(0, &rootentry, file);
		}
	}

	if ((flags & FDIRECTORY) && !fileisdir(file)) {
		errno = ENOTDIR;
		return false;
	}

	// if the file is a directory and the user didn't as for one, error
	if (fileisdir(file) && !(flags & FDIRECTORY)) {
		errno = EISDIR;
		return false;
	}

	file->opened = 1;

	if (flags & FAPPEND)
		fileseek(file, file->fsentry.filesize);

	return true;

}

int32_t _fileread(struct file *file, void *buffer, uint32_t size)
{
	uint32_t sectorpos, newsectorstoread;
	uint8_t *bytebuffer = (uint8_t *) buffer;
	uint32_t filetotalbytes = file->totalclusters * _vbootsector->sectorspercluster * _vbootsector->bytespersector;

	if (!fileisdir(file))
		filetotalbytes = umin(filetotalbytes, file->fsentry.filesize);

	size = umin(size, filetotalbytes - file->position);
	uint32_t bytesleft = size;

	sectorpos = file->position % SECTOR_SIZE;
	newsectorstoread = (sectorpos + size) / SECTOR_SIZE;

	while (newsectorstoread--)
	{
		memcpy(bytebuffer, &file->buffer[sectorpos], SECTOR_SIZE - sectorpos);
		file->position += SECTOR_SIZE - sectorpos;
		bytebuffer += SECTOR_SIZE - sectorpos;
		bytesleft -= SECTOR_SIZE - sectorpos;
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

		// TODO: return number of bytes read if read fails
		fatreadsector(file->currentcluster, file->sectorincluster, file->buffer);

	}

	if (bytesleft != 0) {
		memcpy(bytebuffer, &file->buffer[sectorpos], bytesleft);
		file->position += bytesleft;
	}
	return size;
}

int32_t fileread(struct file *file, void *buffer, uint32_t size) {
	if (file == NULL || buffer == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (file->opened == 0) {
		errno = EBADF;
		return -1;
	}

	// TODO: is this right??? we're erroring if we try to read a read-only file...
	if (file->fsentry.attributes & READ_ONLY) {
		errno = EACCES;
		return -1;
	}

	if (size == 0) return 0;

	return _fileread(file, buffer, size);
}

int32_t _filewrite(struct file *file, const void *buffer, uint32_t size) {
	const uint8_t *bytebuffer = (const uint8_t *) buffer;

	uint32_t maxpossiblebytes = file->position + size;

	// increase allocated file disk space if necessary
	uint32_t currentcapacity = file->totalclusters * _vbootsector->sectorspercluster * _vbootsector->bytespersector;
	if (maxpossiblebytes > currentcapacity) fileresize(file, maxpossiblebytes);

	uint32_t bytesleft = size;
	uint32_t sectorpos = file->position % SECTOR_SIZE;
	int newsectorstowrite = (sectorpos + size) / SECTOR_SIZE;

	// TODO: is this right??? needs more comments.
	// write to buffer
	do {
		uint32_t bytestowrite = umin(bytesleft, SECTOR_SIZE - sectorpos);

		memcpy(file->buffer + sectorpos, bytebuffer, bytestowrite);
		file->dirty = 1;
		file->position += bytestowrite;
		bytebuffer += bytestowrite;
		bytesleft  -= bytestowrite;
		sectorpos = 0;

		// flush bytes if we need more space, and return the number written if flushing fails
		if (bytesleft > 0 && !fileflush(file))
			return bytesleft + bytestowrite;

	} while ((newsectorstowrite--) > 0);

	// update file size
	// if directory, size == clusters * bytes/cluster
	// else file size == sizeof contents
	file->fsentry.filesize = fileisdir(file) ? file->totalclusters * _vbootsector->sectorspercluster * _vbootsector->bytespersector : umax(file->fsentry.filesize, file->position);

	return size;
}

int32_t filewrite(struct file *file, const void *buffer, uint32_t size) {
	if (file == NULL || buffer == NULL) {
		errno = EINVAL;
		return false;
	}

	if (file->opened == 0){
		errno = EBADF;
		return false;
	}

	if (size == 0) return true;

	// TODO: we're not checking if file is read-only...

	return _filewrite(file, buffer, size);
}

bool filereset(struct file *file)
{

	if (file == NULL) {
		errno = EINVAL;
		return false;
	}

	if (file->opened == 0) {
		errno = EBADF;
		return false;
	}

	/* read new sector if the current buffer isn't the very first sector */
	if (file->position >= SECTOR_SIZE)
		fatreadsector(file->firstcluster, 0, file->buffer);

	file->position = 0;
    file->currentcluster = file->firstcluster;
    file->sectorincluster = 0;

    return true;

}

bool fileseek(struct file *file, uint32_t seek) {

	if (file == NULL || seek > file->fsentry.filesize) {
		errno = EINVAL;
		return false;
	}

	if (file->opened == 0) {
		errno = EBADF;
		return false;
	}

	uint32_t clustersin = seek / (_vbootsector->sectorspercluster * _vbootsector->bytespersector);
	uint32_t newsector = (seek / _vbootsector->bytespersector) % _vbootsector->sectorspercluster;

	// get the cluster our target is in
	uint32_t newcluster = file->firstcluster;
	while (clustersin--) 
		newcluster = fatclustervalue(newcluster);

	if (file->currentcluster != newcluster || file->sectorincluster != newsector)
		fatreadsector(newcluster, newsector, file->buffer);

	file->position = seek;
	file->currentcluster = newcluster;
	file->sectorincluster = newsector;

	return true;
}

bool fileflush(struct file *file) {

	if (file == NULL) {
		errno = EINVAL;
		return false;
	}

	if (file->opened == 0) {
		errno = EBADF;
		return false;
	}

	if (!file->dirty) return true;

	// flush to disk
	if (!fatwritesector(file->currentcluster, file->sectorincluster, file->buffer))
		return false;

	// update sector and cluster values
	file->sectorincluster++;
	if (file->sectorincluster >= _vbootsector->sectorspercluster) {
		file->sectorincluster = 0;
		file->currentcluster = fatclustervalue(file->currentcluster);
	}
	
	// file no longer dirty
	file->dirty = false;

	return true;
}

bool fileclose(struct file *file) {

	if (file == NULL)  {
		errno = EINVAL;
		return false;
	}

	if (file->opened == 0) {
		errno = EBADF;
		return false;
	}

	// if we need to update entry in parent
	if (file->dirty && file->parentdircluster != 0) {

		struct fatdirentry parentfakeentry = { 
			.attributes = DIRECTORY, 
			.firstclusterlo = file->parentdircluster, 
			.firstclusterhi = file->parentdircluster >> 16
		};

		// open parent dir
		struct file parentdir;
		filenew(0, &parentfakeentry, &parentdir);

		// update entry in parent
		dirupdateentry(&parentdir, &file->fsentry);

		// close parent dir
		// (we don't fileclose because that would loop up to root dir, which isn't necessary)
		// (and dirupdateentry flushes automatically)
		parentdir.opened = false;

	}

	fileflush(file);

	file->opened = false;

	return true;
}

uint32_t filesize(struct file *file) {
	if (file == NULL) {
		errno = EINVAL;
		return 0;
	}

	if (file->opened == 0) {
		errno = EBADF;
		return 0;
	}

	return file->fsentry.filesize;
}

uint32_t filetell(struct file *file) {
	if (file == NULL) {
		errno = EINVAL;
		return 0;
	}

	if (file->opened == 0) {
		errno = EBADF;
		return 0;
	}

	return file->position;
}
