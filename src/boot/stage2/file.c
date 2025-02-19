#include "file.h"
#include "std.h"
#include "write.h"

struct file root;

int32_t _fileread(struct file *file, void *buffer, uint32_t size);

void filenew(struct fatdirentry *entry, struct file *fileout) {
	memcpy(&fileout->fsentry, entry, sizeof(struct fatdirentry));
	fileout->opened = true;
	fileout->position = 0;
	fileout->firstcluster = entry->firstclusterlo | (uint32_t) entry->firstclusterhi << 16;
	fileout->totalclusters = fattotalclusters(fileout->firstcluster, &fileout->lastcluster);
	fileout->currentcluster = fileout->firstcluster;
    fileout->sectorincluster = 0;
	fatreadsector(fileout->firstcluster, 0, fileout->buffer);
}

int32_t findentry(struct file *directory, const char *name, struct file *fileout) {
	struct fatdirentry entry;
	int possibleentries = (directory->totalclusters * _bootsector->sectorspercluster * _bootsector->bytespersector) 
								/ sizeof(struct fatdirentry);

	if (!fileisdir(directory))
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

uint32_t pathnext(char **start) {
	char *cursor = *start;
	uint32_t size = 0;
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

bool fileopen(struct file *file, const char *pathname, int flags) {

	UNUSED(flags);

	memcpy(file, &root, sizeof(struct file));

	int pathnextsize = 0;

	char *start = (char *) pathname;
	char fatformattedname[FAT_FILETOTAL_LEN];

	if (pathname == NULL || pathname[0] != PATH_SEP)
		return false;


	while ((pathnextsize = pathnext(&start))) {

		fatformatfilename(start, pathnextsize, fatformattedname);

		if (findentry(file, fatformattedname, file) < 0)
			return false;

	}

	file->opened = true;

	return true;

}

int32_t _fileread(struct file *file, void *buffer, uint32_t size) {
	uint32_t sectorpos, newsectorstoread;
	uint8_t *bytebuffer = (uint8_t *) buffer;
	uint32_t filetotalbytes = file->totalclusters * _bootsector->sectorspercluster * _bootsector->bytespersector;

	if (!fileisdir(file))
		filetotalbytes = umin(filetotalbytes, file->fsentry.filesize);

	size = umin(size, filetotalbytes - file->position);

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

int32_t fileread(struct file *file, void *buffer, uint32_t size) {
	
	if (file == NULL || file->opened == 0)
		return -1;

	return _fileread(file, buffer, size);

}

bool filereset(struct file *file) {

	if (file == NULL || file->opened == 0)
		return false;

	// read new sector if the current buffer isn't the very first sector
	if (file->position >= SECTOR_SIZE)
		fatreadsector(file->firstcluster, 0, file->buffer);

	file->position = 0;
    file->currentcluster = file->firstcluster;
    file->sectorincluster = 0;

    return true;

}

bool fileseek(struct file *file, uint32_t seek) {

	if (file == NULL || file->opened == 0 || seek >= file->fsentry.filesize)
		return false;

	uint32_t clustersin = seek / (_bootsector->sectorspercluster * _bootsector->bytespersector);
	uint32_t newsector = (seek / _bootsector->bytespersector) % _bootsector->sectorspercluster;

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
