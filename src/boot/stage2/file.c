#include "file.h"
#include "std.h"

int errno;
const char *strerror[] =
{
	"NO SUCH FILE OR DIRECTORY",
	"BAD FILE DESCRIPTOR",
	"TRIED TO READ SOMETHING NOT A DIRECTORY AS A DIRECTORY",
	"INVALID ARGUMENT",
	"BAD FILE PATH"
};

struct file root;

int _fileread(struct file *file, void *buffer, uint size);

void load(uint cluster, uint size, int isdirectory, struct file *fileout)
{
	fileout->opened = 1;
	fileout->isdirectory = isdirectory;
	fileout->position = 0;
	fileout->size = size;
	fileout->totalclusters = fattotalclusters(cluster);
	fileout->firstcluster = cluster;
	fileout->currentcluster = cluster;
    fileout->sectorincluster = 0;
	fatreadsector(cluster, 0, fileout->buffer);
}

int findentry(struct file *directory, const char *name, struct file *fileout)
{
	struct fatdirentry entry;
	uint cluster = 0;
	int isdirectory = 0;
	int possibleentries = (directory->totalclusters * _bootsector->sectorspercluster * _bootsector->bytespersector) 
								/ sizeof(struct fatdirentry);

	if (directory->isdirectory == 0)
	{
		errno = ENOTDIR;
		return -1;
	}

	/* make sure we start from the beginning when reading a directory */
	if (directory->position != 0) 
		filereset(directory);

	do
	{
		int readresult = _fileread(directory, &entry, sizeof(struct fatdirentry));
		if (readresult < 0)
			return readresult;
		if (strncmp((char *) entry.filename, name, FAT_FILETOTAL_LEN) == 0)
		{
			cluster = entry.firstclusterlo | ((uint)entry.firstclusterhi << 16);
			isdirectory = (entry.attributes & DIRECTORY) != 0;
			load(cluster, entry.filesize, isdirectory, fileout);
			fileout->attributes = entry.attributes;
			return 0;
		}
	}
	while(fatnomoredirentry(&entry) == 0 && possibleentries--);

	errno = ENOENT;
	return -1;
}

uint pathnext(char **start)
{
	char *cursor = *start;
	uint size = 0;
	while (*cursor != PATH_SEP && *cursor) 
	{
		(*start)++;
		cursor++;
	}
	while (*cursor == PATH_SEP) 
	{	
		(*start)++;
		cursor++;
	}
	while (*cursor && *cursor != PATH_SEP) 
	{
		size++;
		cursor++;
	}
	return size;
}

void fileinit(void)
{
	load(2, 0, 1, &root);
}

int fileopen(struct file *file, const char *pathname)
{
	/* todo: try to only use the provided file for this */
	struct file activedir = root;

	int pathnextsize, status = 0;

	char *start = (char *) pathname;
	char fatformattedname[FAT_FILETOTAL_LEN];

	if (pathname == NULL || pathname[0] != PATH_SEP)
	{
		errno = EPATH;
		return -1;
	}

	pathnextsize = pathnext(&start);

	while (pathnextsize)
	{

		fatformatfilename(start, pathnextsize, fatformattedname);
		status = findentry(&activedir, fatformattedname, &activedir);
		if (status < 0)
			return status;

		pathnextsize = pathnext(&start);

	}

	memcpy(file, &activedir, sizeof(struct file));
	file->opened = 1;

	return 0;

}

int _fileread(struct file *file, void *buffer, uint size)
{
	uint sectorpos, newsectorstoread;
	uchar *bytebuffer = (uchar *) buffer;
	uint filetotalbytes = file->totalclusters * _bootsector->sectorspercluster * _bootsector->bytespersector;

	if (!file->isdirectory)
		filetotalbytes = min(filetotalbytes, file->size);

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
			if (file->currentcluster == FEEND32)
			{
				/* reached eof */
				file->position = file->size;
				return 0;
			}
		}

		fatreadsector(file->currentcluster, file->sectorincluster, file->buffer);

	}

	memcpy(bytebuffer, &file->buffer[sectorpos], size);
	file->position += size;
	return size;
}

int fileread(struct file *file, void *buffer, uint size)
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

	return _fileread(file, buffer, size);

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

int fileseek(struct file *file, uint seek)
{

	if (file == NULL || seek >= file->size) 
	{
		errno = EINVAL;
		return -1;
	}

	if (file->opened == 0)
	{
		errno = EBADF;
		return -1;
	}

	uint clustersin = seek / (_bootsector->sectorspercluster * _bootsector->bytespersector);
	uint newsector = (seek / _bootsector->bytespersector) % _bootsector->sectorspercluster;

	uint newcluster = file->firstcluster;
	while (clustersin--) 
	{
		newcluster = fatclustervalue(newcluster);
	}

	if (file->currentcluster != newcluster || file->sectorincluster != newsector)
		fatreadsector(newcluster, newsector, file->buffer);

	file->position = seek;
	file->currentcluster = newcluster;
	file->sectorincluster = newsector;

	return 0;
}
