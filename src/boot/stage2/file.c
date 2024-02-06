#include "file.h"
#include "std.h"
#include "write.h"

struct file root;

int _fileread(struct file *file, void *buffer, uint size);

void filenew(struct fatdirentry *entry, struct file *fileout) {
	memcpy(&fileout->fsentry, entry, sizeof(struct fatdirentry));
	fileout->opened = 1;
	fileout->position = 0;
	fileout->firstcluster = entry->firstclusterlo | (uint) entry->firstclusterhi << 16;
	fileout->totalclusters = fattotalclusters(fileout->firstcluster, &fileout->lastcluster);
	fileout->currentcluster = fileout->firstcluster;
    fileout->sectorincluster = 0;
	fatreadsector(fileout->firstcluster, 0, fileout->buffer);
}

int findentry(struct file *directory, const char *name, struct file *fileout) {
	struct fatdirentry entry;
	int possibleentries = (directory->totalclusters * _bootsector->sectorspercluster * _bootsector->bytespersector) 
								/ sizeof(struct fatdirentry);

	if ((directory->fsentry.attributes & DIRECTORY) == 0)
		return -1;

	/* make sure we start from the beginning when reading a directory */
	if (directory->position != 0) 
		filereset(directory);

	do {
		int readresult = _fileread(directory, &entry, sizeof(struct fatdirentry));
		if (readresult < 0)
			return readresult;
		if (strncmp((char *) entry.filename, name, FAT_FILETOTAL_LEN) == 0) {
			filenew(&entry, fileout);
			return 0;
		}
	}
	while(fatnomoredirentry(&entry) == 0 && possibleentries--);

	return -1;
}

uint pathnext(char **start) {
	char *cursor = *start;
	uint size = 0;
	// move past current path
	while (*cursor != PATH_SEP && *cursor) 
	{
		(*start)++;
		cursor++;
	}
	// ignore multiple path sep
	while (*cursor == PATH_SEP) 
	{	
		(*start)++;
		cursor++;
	}
	// *start is where is should be, continue to
	// find the size of the path
	while (*cursor != PATH_SEP && *cursor) 
	{
		size++;
		cursor++;
	}
	return size;
}

void fileinit(void) {
	struct fatdirentry rootentry = FAT_PHYS_ROOT_ENTRY;
	filenew(&rootentry, &root);
}

int fileopen(struct file *file, const char *pathname, int flags) {

	UNUSED(flags);

	memcpy(file, &root, sizeof(struct file));

	int pathnextsize = 0;

	char *start = (char *) pathname;
	char fatformattedname[FAT_FILETOTAL_LEN];

	if (pathname == NULL || pathname[0] != PATH_SEP)
		return -1;


	while ((pathnextsize = pathnext(&start))) {

		fatformatfilename(start, pathnextsize, fatformattedname);

		if (findentry(file, fatformattedname, file) < 0)
			return -1;

	}

	file->opened = 1;

	return 0;

}

int _fileread(struct file *file, void *buffer, uint size) {
	uint sectorpos, newsectorstoread;
	uchar *bytebuffer = (uchar *) buffer;
	uint filetotalbytes = file->totalclusters * _bootsector->sectorspercluster * _bootsector->bytespersector;

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
		if (file->sectorincluster >= _bootsector->sectorspercluster)
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

	memcpy(bytebuffer, &file->buffer[sectorpos], size);
	file->position += size;
	return size;
}

int fileread(struct file *file, void *buffer, uint size) {
	
	if (file == NULL || file->opened == 0)
		return -1;

	return _fileread(file, buffer, size);

}

int filereset(struct file *file) {

	if (file == NULL || file->opened == 0)
		return -1;

	// read new sector if the current buffer isn't the very first sector
	if (file->position >= SECTOR_SIZE)
		fatreadsector(file->firstcluster, 0, file->buffer);

	file->position = 0;
    file->currentcluster = file->firstcluster;
    file->sectorincluster = 0;

    return 0;

}

void printint(uint x, int base, int sign) {
	
	const char digits[] = "0123456789ABCDEF";
	char buffer[16];
	int negative = 0;

	if (sign && (* (int *) &x) < 0) {
		negative = 1;
		x = -x;
	}

	int i = 0;
	do {
		buffer[i++] = digits[x % base];
	} while ((x /= base) != 0);

	if (negative) buffer[i++] = '-';

	while(--i >= 0)
    	putc(buffer[i]);

}

int fileseek(struct file *file, uint seek) {

	if (file == NULL || file->opened == 0 || seek >= file->fsentry.filesize)
		return -1;

	uint clustersin = seek / (_bootsector->sectorspercluster * _bootsector->bytespersector);
	uint newsector = (seek / _bootsector->bytespersector) % _bootsector->sectorspercluster;

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
