#include "fat.h"
#include "write.h"
#include "ata.h"
#include "std.h"

char FAT_VALID_FILENAME_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!#$%&'()-@^_`{}~";

uint _fatsector[FAT_COUNT];
struct fatbootsector *_bootsector;

uint _cachefirstentry;
ENTRY_TYPE _fatcache[ENTRIES_PER_SECTOR];

void fatinit(void *bootsectoraddress)
{

	_bootsector = (struct fatbootsector *) bootsectoraddress;

	if (_bootsector->fatcount > FAT_COUNT)
		puterr("Too many fats, only handling 2\n", 1);

	if (_bootsector->fatsize16 != 0)
		puterr("Only fat32 supported, found fat12/fat16\n", 0);

	/* this is the start of the first fat */
	_fatsector[0] = _bootsector->reservedsectors;

	for (int i = 1; i < FAT_COUNT; i++)
	{
		_fatsector[i] = _fatsector[i - 1] + _bootsector->ex.fat32.fatsize32;
	}

	fatcache(0, 0);

}

uint fatclustertolba(ENTRY_TYPE cluster)
{
	uint dataregionstart = _bootsector->reservedsectors + (_bootsector->fatcount * _bootsector->ex.fat32.fatsize32);
	return ((cluster - _bootsector->ex.fat32.rootcluster) * _bootsector->sectorspercluster) + dataregionstart;
}

int fatnomoredirentry(struct fatdirentry *direntry)
{
	return direntry->filename[0] == 0x00;
}

void fatreadsector(ENTRY_TYPE cluster, uint sector, void *out)
{
	uint clusterlba = fatclustertolba(cluster);
	ataread(clusterlba + sector, 1, out);
}

int fatcache(uint fat, uint offsetsector)
{
	if (fat >= FAT_COUNT)
		return -1;

	if (offsetsector >= _bootsector->ex.fat32.fatsize32)
		return -1;

	ataread(_fatsector[fat] + offsetsector, 1, _fatcache);
	_cachefirstentry = offsetsector * ENTRIES_PER_SECTOR;

	return 0;
}

uint fattotalclusters(uint entryposition)
{
	uint count = 0;
	ENTRY_TYPE value = entryposition;
	do
	{
		value = fatclustervalue(value);
		count++;
	}
	while (value != FEEND32);
	
	return count;
}

ENTRY_TYPE fatclustervalue(uint entryposition)
{
	if (entryposition < _cachefirstentry || entryposition >= _cachefirstentry + ENTRIES_PER_SECTOR)
	{
		fatcache(0, entryposition / ENTRIES_PER_SECTOR);
	}
	return _fatcache[entryposition % ENTRIES_PER_SECTOR];
}

/* See https://en.wikipedia.org/wiki/8.3_filename */

int _fatformatchar(int c)
{
	char *valid = strchr(FAT_VALID_FILENAME_CHARS, c);

	if (valid || c >= 128)
		return c;

	if (c >= 'a' && c <= 'z')
		return c -= 0x20;

	if (c == 0xE5)
		return 0x05;

	return FAT_DEFAULT_FILENAME_CHAR;
}

int _fatformatfilename(const char *name, char *dest, uint max)
{

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

void fatformatfilename(const char *input, uint inputlength, char *output)
{

	const char *dotlocation = strnchr(input, '.', inputlength);
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

	if (dotlocation)
	{
		_fatformatfilename(dotlocation + 1, output + FAT_FILENAME_LEN, FAT_FILEEXT_LEN);
		inputlength -= strlen(dotlocation);
	}

	_fatformatfilename(input, output, min(FAT_FILENAME_LEN, inputlength));

}





