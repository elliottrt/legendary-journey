#include "fat.h"
#include "write.h"
#include "ata.h"
#include "std.h"

// UNUSED
int fatwritesector(fat_entry_t cluster, uint sector, const void *data);

char FAT_VALID_FILENAME_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!#$%&'()-@^_`{}~";

uint _first_fat;

uint _cachefirstentry;
fat_entry_t _fatcache[ENTRIES_PER_SECTOR];

void fatinit(void) {

	if (_bootsector->fatsize16 != 0)
		puterr("Only fat32 supported, found fat12/fat16\n", 0);

	/* this is the start of the first fat */
	_first_fat = _bootsector->reservedsectors;

	fatcache(0, 0);

}

uint fatclustertolba(fat_entry_t cluster) {
	uint dataregionstart = _bootsector->reservedsectors + (_bootsector->fatcount * _bootsector->fatsize32);
	return ((cluster - _bootsector->rootcluster) * _bootsector->sectorspercluster) + dataregionstart;
}

int fatnomoredirentry(struct fatdirentry *direntry) {
	return direntry == NULL || direntry->filename[0] == 0x00;
}

int fatreadsector(fat_entry_t cluster, uint sector, void *out) {
	uint clusterlba = fatclustertolba(cluster);
	return ataread(clusterlba + sector, 1, out);
}

int fatcache(uint fat, uint offsetsector) {
	UNUSED(fat);

	if (offsetsector >= _bootsector->fatsize32)
		return -1;

	ataread(_first_fat + offsetsector, 1, _fatcache);
	_cachefirstentry = offsetsector * ENTRIES_PER_SECTOR;

	return 0;
}

uint fattotalclusters(fat_entry_t cluster, uint *lastcluster) {

	uint count = 0;

	fat_entry_t value = cluster;
	do {
		if (lastcluster) *lastcluster = value;
		value = fatclustervalue(value);
		count++;
	} while (value < FEEND32L);
	
	return count;
}

fat_entry_t fatclustervalue(uint entryposition) {
	if (entryposition < _cachefirstentry || entryposition >= _cachefirstentry + ENTRIES_PER_SECTOR)
		fatcache(0, entryposition / ENTRIES_PER_SECTOR);

	return _fatcache[entryposition % ENTRIES_PER_SECTOR];
}

/* See https://en.wikipedia.org/wiki/8.3_filename */

int _fatformatchar(int c) {
	char *valid = strchr(FAT_VALID_FILENAME_CHARS, c);

	if (valid || c >= 128)
		return c;

	if (c >= 'a' && c <= 'z')
		return c -= 0x20;

	if (c == 0xE5)
		return 0x05;

	return FAT_DEFAULT_FILENAME_CHAR;
}

int _fatformatfilename(const char *name, char *dest, uint max) {

	if (dest == 0)
		return -1;

	/* memset(dest, FAT_DEFAULT_FILENAME_PADDING, max); */

	if (name == 0)
		return -1;

	while (*name == ' ') 
		name++;

	while(*name && max--)
	{
		*dest = _fatformatchar(*name);

		dest++;
		name++;
	}

	return 0;
}

void fatformatfilename(const char *input, uint inputlength, char *output) {

	memset(output, FAT_DEFAULT_FILENAME_PADDING, FAT_FILETOTAL_LEN);

	if (memcmp(input, FAT_DOT, FAT_FILETOTAL_LEN) == 0)
	{
		memcpy(output, FAT_DOT, FAT_FILETOTAL_LEN);
		return;
	}
	if (memcmp(input, FAT_DOTDOT, FAT_FILETOTAL_LEN) == 0)
	{
		memcpy(output, FAT_DOTDOT, FAT_FILETOTAL_LEN);
		return;
	}

	const char *dotlocation = strnchr(input, '.', inputlength);

	if (dotlocation)
	{
		_fatformatfilename(dotlocation + 1, output + FAT_FILENAME_LEN, FAT_FILEEXT_LEN);
		inputlength -= strlen(dotlocation);
	}

	_fatformatfilename(input, output, min(FAT_FILENAME_LEN, inputlength));

}





