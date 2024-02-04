#include "fat.h"
#include "ata.h"
#include "std.h"
#include "printf.h"

char FAT_VALID_FILENAME_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!#$%&'()-@^_`{}~";

uint _fatsector[FAT_COUNT];

static struct fatsearch {
	uint start;
} _fatsearch;

// TODO: dynamically allocate _fatcache.buffer and increase its size
static struct fatcache {
	uchar dirty;
	uint firstentry;
	uint startingsector;
	uchar sectorsbuffered;
	fat_entry_t buffer[ENTRIES_PER_SECTOR * FAT_CACHE_SIZE];
} _fatcache;

void fatinit(void) {

	if (_vbootsector->fatcount > FAT_COUNT) {
		printferr();
		printf("too many fats (%d), only handling %d\n", _vbootsector->fatcount, FAT_COUNT);
		printfstd();
	}

	// this is the start of the first fat
	_fatsector[0] = _vbootsector->reservedsectors;

	for (int i = 1; i < FAT_COUNT; i++)
		_fatsector[i] = _fatsector[i - 1] + _vbootsector->fatsize32;

	// start searching after reserved clusters
	_fatsearch.start = 2;

	fatcache(0, 0);

}

uint fatclustertolba(fat_entry_t cluster) {
	uint dataregionstart = _vbootsector->reservedsectors + (_vbootsector->fatcount * _vbootsector->fatsize32);
	return ((cluster - _vbootsector->rootcluster) * _vbootsector->sectorspercluster) + dataregionstart;
}

int fatnomoredirentry(struct fatdirentry *direntry) {
	return direntry == NULL || direntry->filename[0] == 0x00;
}

int fatreadsector(fat_entry_t cluster, uint sector, void *data) {
	uint clusterlba = fatclustertolba(cluster);
	return ataread(clusterlba + sector, 1, data);
}

int fatwritesector(fat_entry_t cluster, uint sector, const void *data) {
	uint clusterlba = fatclustertolba(cluster);
	return atawrite(clusterlba + sector, 1, data);
}

int fatcache(uint fat, uint offsetsector) {
	if (fat >= FAT_COUNT || offsetsector >= _vbootsector->fatsize32)
		return -1;

	// write back to all fats if dirty
	fatcommit();

	_fatcache.sectorsbuffered = min(FAT_CACHE_SIZE, _vbootsector->fatsize32 - offsetsector);

	ataread(_fatsector[fat] + offsetsector, _fatcache.sectorsbuffered, _fatcache.buffer);

	_fatcache.dirty = 0;
	_fatcache.firstentry = offsetsector * ENTRIES_PER_SECTOR;
	_fatcache.startingsector = offsetsector;

	return 0;
}

void fatcommit(void) {
	if (_fatcache.dirty) {
		for (uint fatidx = 0; fatidx < FAT_COUNT; ++fatidx)
			atawrite(_fatsector[fatidx] + _fatcache.startingsector, _fatcache.sectorsbuffered, _fatcache.buffer);
	}
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
	if (entryposition < _fatcache.firstentry || entryposition >= _fatcache.firstentry + (ENTRIES_PER_SECTOR * _fatcache.sectorsbuffered))
		fatcache(0, entryposition / ENTRIES_PER_SECTOR);

	return _fatcache.buffer[entryposition % (ENTRIES_PER_SECTOR * _fatcache.sectorsbuffered)];
}

void fatsetcluster(uint cluster, fat_entry_t value) {
	if (cluster < _fatcache.firstentry || cluster >= _fatcache.firstentry + (ENTRIES_PER_SECTOR * _fatcache.sectorsbuffered))
		fatcache(0, cluster / ENTRIES_PER_SECTOR);

	_fatcache.dirty = 1;
	_fatcache.buffer[cluster % (ENTRIES_PER_SECTOR * _fatcache.sectorsbuffered)] = value;
	
	if (value == FEFREE && cluster < _fatsearch.start)
		_fatsearch.start = cluster - 1;
}

uint fatfindfreecluster(void) {

	// start where we are certain there is nothing open behind
	uint current = _fatsearch.start;

	uint possible_entries_left = (_vbootsector->fatsize32 * _vbootsector->bytespersector / sizeof(fat_entry_t)) - current * sizeof(fat_entry_t);
	
	// TODO: set a cap value on possible_entries_left
	// because it will search the whole fat for a free
	// spot if it needs to

	while (possible_entries_left--) {
		fat_entry_t value = fatclustervalue(current);
		if (value == FEFREE) {
			_fatsearch.start = current + 1;
			return current;
		}
		current++;
	}

	printferr();
	printf("run out of disk space");
	printfstd();

	_fatsearch.start = _vbootsector->fatsize32 * _vbootsector->bytespersector / sizeof(fat_entry_t);

	return 0;
}

fat_entry_t fatallocclusters(fat_entry_t start, uint count) {
	if (count == 0) return 0;

	uint clustersleft = count;
	fat_entry_t top = start;
	while (clustersleft--) {
		uint freecluster = fatfindfreecluster();
		fatsetcluster(top, freecluster);
		top = freecluster;
	}

	fatsetcluster(top, FEEND32L);

	return top;
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

	if (dest == 0 || name == 0)
		return -1;

	while (*name == ' ') 
		name++;

	while(*name && max--) {
		*dest = _fatformatchar(*name);

		dest++;
		name++;
	}

	return 0;
}

void fatformatfilename(const char *input, uint inputlength, char *output) {

	memset(output, FAT_DEFAULT_FILENAME_PADDING, FAT_FILETOTAL_LEN);

	if (memcmp(input, FAT_DOT, FAT_FILETOTAL_LEN) == 0 || (inputlength == 1 && input[0] == '.')) {
		memcpy(output, FAT_DOT, FAT_FILETOTAL_LEN);
		return;
	}
	if (memcmp(input, FAT_DOTDOT, FAT_FILETOTAL_LEN) == 0 || (inputlength == 2 && input[0] == '.' && input[1] == '.')) {
		memcpy(output, FAT_DOTDOT, FAT_FILETOTAL_LEN);
		return;
	}
	
	const char *dotlocation = strnchr(input, '.', inputlength);

	if (dotlocation) {
		_fatformatfilename(dotlocation + 1, output + FAT_FILENAME_LEN, FAT_FILEEXT_LEN);
		inputlength -= strlen(dotlocation);
	}

	_fatformatfilename(input, output, min(FAT_FILENAME_LEN, inputlength));

}
