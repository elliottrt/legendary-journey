#include "fat.h"
#include "../write.h"
#include "../std.h"
#include "ata.h"

#define FATCOUNT 2
uint32_t _FATSECTOR[FATCOUNT] = {0};
struct fatbootsector *_fboot = NULL;

uint32_t _CACHEFIRSTENTRY = 0;
ENTRY_TYPE _FATCACHE[ENTRIES_PER_SECTOR];

uint32_t NEXTOPENClUSTER = 0;

void fat_init(void *boot_sector_address)
{
	uint32_t i;

	_fboot = (struct fatbootsector *) boot_sector_address;

	if (_fboot->fat_count > FATCOUNT)
		puterr("Too many FATs, only handling 2\n", 1);

	if (_fboot->fat_size_16 != 0)
		puterr("Only FAT32 supported, found FAT12/FAT16\n", 0);

	/* this is the start of the first fat */
	_FATSECTOR[0] = _fboot->reserved_sectors;

	for (i = 1; i < FATCOUNT; i++)
	{
		_FATSECTOR[i] = _FATSECTOR[i - 1] + _fboot->ex.fat32.fat_size_32;
	}

	fat_cache(0, 0);

}

uint32_t fat_cluster_to_lba(ENTRY_TYPE cluster)
{
	uint32_t data_region_start = _fboot->reserved_sectors + (_fboot->fat_count * _fboot->ex.fat32.fat_size_32);
	return ((cluster - _fboot->ex.fat32.root_cluster) * _fboot->sectors_per_cluster) + data_region_start;
}

ENTRY_TYPE fat_direntry_cluster(struct fatdirentry *direntry)
{
	return direntry->first_cluster_lo + (((uint32_t)direntry->first_cluster_hi) << 16);
}

int fat_free_direntry(struct fatdirentry *direntry)
{
	return direntry->file_name[0] == 0xE5 || direntry->file_name[0] == 0x00;
}

int fat_no_more_direntry(struct fatdirentry *direntry)
{
	return direntry->file_name[0] == 0x00;
}

void fat_read_cluster(ENTRY_TYPE cluster, void *out)
{
	uint32_t lba = fat_cluster_to_lba(cluster);
	ata_read(lba, _fboot->sectors_per_cluster, out);
}

void fat_read_sector(ENTRY_TYPE cluster, uint32_t sector, void *out)
{
	uint32_t lba = fat_cluster_to_lba(cluster);
	lba += sector;
	ata_read(lba, 1, out);
}

int32_t fat_cache(uint32_t fat, uint32_t offset_sector)
{
	if (fat >= FATCOUNT)
		return -1;

	if (offset_sector >= _fboot->ex.fat32.fat_size_32)
		return -1;

	ata_read(_FATSECTOR[fat] + offset_sector, 1, _FATCACHE);
	_CACHEFIRSTENTRY = offset_sector * ENTRIES_PER_SECTOR;

	return 0;
}

uint32_t fat_total_clusters(uint32_t entry_position)
{
	uint32_t count = 0;
	ENTRY_TYPE value = entry_position;
	do
	{
		value = fat_cluster_value(value);
		count++;
	}
	while (value != FEEND32);
	
	return count;
}

ENTRY_TYPE fat_cluster_value(uint32_t entry_position)
{
	if (entry_position < _CACHEFIRSTENTRY || entry_position >= _CACHEFIRSTENTRY + ENTRIES_PER_SECTOR)
	{
		fat_cache(0, entry_position / ENTRIES_PER_SECTOR);
	}
	return _FATCACHE[entry_position % ENTRIES_PER_SECTOR];
}

char FAT_VALID_FILENAME_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!#$%&'()-@^_`{}~";
#define FAT_DEFAULT_FILENAME_CHAR '_'
#define FAT_DEFAULT_FILENAME_PADDING ' '

/* See https://en.wikipedia.org/wiki/8.3_filename */

int fat_format_char(int c)
{
	char *valid = strchr(FAT_VALID_FILENAME_CHARS, c);

	if (valid || c >= 128)
		return c;

	if (islower(c))
		return toupper(c);

	if (c == 0xE5)
		return 0x05;

	return FAT_DEFAULT_FILENAME_CHAR;
}

int32_t _fat_format_filename(const char *name, char *dest, uint32_t max)
{

	if (dest == NULL)
		return -1;

	memset(dest, FAT_DEFAULT_FILENAME_PADDING, max);

	if (name == NULL)
		return -1;

	while (*name == ' ') 
		name++;

	while(*name && max--)
	{
		*dest = fat_format_char(*name);

		dest++;
		name++;
	}

	return 0;
}

void fat_format_filename(const char *input, uint32_t input_length, char *output)
{

	const char *dot_location = strnchr(input, '.', input_length);
	memset(output, FAT_DEFAULT_FILENAME_PADDING, FAT_FILETOTAL_LEN);

	if (strncmp(input, ".", 1) == 0)
	{
		strncpy(output, ".          ", 11);
		return;
	}
	if (strncmp(input, "..", 2) == 0)
	{
		strncpy(output, "..         ", 11);
		return;
	}

	if (dot_location)
	{
		_fat_format_filename(dot_location + 1, output + FAT_FILENAME_LEN, FAT_FILEEXT_LEN);
		input_length -= strlen(dot_location);
	}

	_fat_format_filename(input, output, min(FAT_FILENAME_LEN, input_length));

}















