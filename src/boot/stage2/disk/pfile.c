#include "pfile.h"
#include "../std.h"

#define DEBUGP(V) \
	puts(#V ": "); \
	putu(V); \
	putc('\n'); \

int32_t errno;
struct pfile root;

int32_t _read(struct pfile *pfile, void *buffer, uint32_t size);

void load(uint32_t cluster, uint32_t size, int32_t is_directory, struct pfile *file_out)
{
	file_out->opened = true;
	file_out->is_directory = is_directory;
	file_out->position = 0;
	file_out->size = size;
	file_out->total_clusters = fat_total_clusters(cluster);
	file_out->first_cluster = cluster;
	file_out->current_cluster = cluster;
    file_out->sector_in_cluster = 0;
	fat_read_sector(cluster, 0, file_out->buffer);
}

int32_t find_entry(struct pfile *directory, const char *name, struct pfile *file_out)
{
	struct fatdirentry entry;
	uint32_t cluster = 0;
	int32_t is_directory = 0;
	int32_t read_result = 0;
	int32_t possible_entries = (directory->total_clusters * _fboot->sectors_per_cluster * _fboot->bytes_per_sector) 
								/ sizeof(struct fatdirentry);

	if (directory->is_directory == false)
	{
		errno = ENOTDIR;
		return -1;
	}

	do
	{
		read_result = _read(directory, &entry, sizeof(struct fatdirentry));
		if (read_result < 0)
			return read_result;
		if (strncmp((char *) entry.file_name, name, FAT_FILETOTAL_LEN) == 0)
		{
			cluster = entry.first_cluster_lo | ((uint32_t)entry.first_cluster_hi << 16);
			is_directory = (entry.attributes & DIRECTORY) != 0;
			load(cluster, entry.file_size, is_directory, file_out);
			file_out->attributes = entry.attributes;
			return 0;
		}
	}
	while(fat_no_more_direntry(&entry) == false && possible_entries--);

	errno = ENOENT;
	return -1;
}

uint32_t path_next(char **start)
{
	char *cursor = *start;
	uint32_t size = 0;
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

void pfile_init(void)
{
	load(2, 0, true, &root);
}

int32_t open(struct pfile *pfile, const char *pathname)
{
	int32_t path_next_size, status = 0;
	/* todo: try to only use the provided pfile for this */
	struct pfile active_dir = root;
	char *start = (char *) pathname;
	char fat_formatted[FAT_FILETOTAL_LEN];

	if (pathname == NULL || pathname[0] != PATH_SEP)
	{
		errno = EPATH;
		return -1;
	}

	if (strlen(pathname) >= MAXPATHLEN)
	{
		errno = ENAMETOOLONG;
		return -1;
	}

	path_next_size = path_next(&start);

	while (path_next_size)
	{

		if (path_next_size > MAXNAMELEN)
		{
			errno = ENAMETOOLONG;
			return -1;
		}

		fat_format_filename(start, path_next_size, fat_formatted);
		status = find_entry(&active_dir, fat_formatted, &active_dir);
		if (status < 0)
			return status;

		path_next_size = path_next(&start);

	}

	memcpy(pfile, &active_dir, sizeof(struct pfile));
	pfile->opened = true;

	return 0;

}

int32_t _read(struct pfile *pfile, void *buffer, uint32_t size)
{
	uint32_t sector_pos, new_sectors_to_read;
	uint8_t *u8_buffer = (uint8_t *) buffer;
	uint32_t file_total_bytes = pfile->total_clusters * _fboot->sectors_per_cluster * _fboot->bytes_per_sector;

	if (!pfile->is_directory)
		file_total_bytes = min(file_total_bytes, pfile->size);

	size = min(size, file_total_bytes - pfile->position);

	sector_pos = pfile->position % SECTOR_SIZE;
	new_sectors_to_read = (sector_pos + size) / SECTOR_SIZE;

	while (new_sectors_to_read--)
	{
		memcpy(u8_buffer, &pfile->buffer[sector_pos], SECTOR_SIZE - sector_pos);
		pfile->position += SECTOR_SIZE - sector_pos;
		u8_buffer += SECTOR_SIZE - sector_pos;
		sector_pos = 0;

		pfile->sector_in_cluster++;
		if (pfile->sector_in_cluster >= _fboot->sectors_per_cluster)
		{
			pfile->sector_in_cluster = 0;
			pfile->current_cluster = fat_cluster_value(pfile->current_cluster);
			if (pfile->current_cluster == FEEND32)
			{
				/* reached eof */
				pfile->position = pfile->size;
				return 0;
			}
		}

		fat_read_sector(pfile->current_cluster, pfile->sector_in_cluster, pfile->buffer);

	}

	memcpy(u8_buffer, &pfile->buffer[sector_pos], size);
	pfile->position += size;
	return size;
}

int32_t read(struct pfile *pfile, void *buffer, uint32_t size)
{
	
	if (pfile == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	if (pfile->opened == false)
	{
		errno = EBADF;
		return -1;
	}

	return _read(pfile, buffer, size);

}

int32_t reset(struct pfile *pfile)
{

	if (pfile == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	if (pfile->opened == false)
	{
		errno = EBADF;
		return -1;
	}

	pfile->position = 0;
    pfile->current_cluster = pfile->first_cluster;
    pfile->sector_in_cluster = 0;
    fat_read_sector(pfile->first_cluster, 0, pfile->buffer);

    return 0;

}
