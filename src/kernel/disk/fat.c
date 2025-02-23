#include "common/fat.h"
#include "common/ata.h"
#include "common/std.h"
#include "kernel/graphics/printf.h"
#include "kernel/memory/kalloc.h"

char FAT_VALID_FILENAME_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!#$%&'()-@^_`{}~";

static struct fatsearch {
	uint32_t start;
} _fatsearch;

static struct fatcache {
	bool dirty;
	uint32_t firstentry;
	uint32_t startingsector;
	uint8_t sectorsbuffered;
	// of size ENTRIES_PER_SECTOR * FAT_CACHE_SIZE
	fat_entry_t *buffer;
} _fatcache;

void fatinit(void) {

	if (_vbootsector->bytespersector * FAT_CACHE_SIZE > PGSIZE) {
		printf("error: fatcache too large (%u bytes), must be <= to %u bytes\n", _vbootsector->bytespersector * FAT_CACHE_SIZE, PGSIZE);
		STOP();
	}

	if ((_fatcache.buffer = (fat_entry_t *) kalloc()) == NULL) {
		printf("error: failed to allocate memory for fat cache\n");
		STOP();
	}

	// start searching after reserved clusters
	_fatsearch.start = 2;

	fatcache(0, 0);

}

uint32_t fatclustertolba(fat_entry_t cluster) {
	uint32_t dataregionstart = _vbootsector->reservedsectors + (_vbootsector->fatcount * _vbootsector->fatsize32);
	return ((cluster - _vbootsector->rootcluster) * _vbootsector->sectorspercluster) + dataregionstart;
}

bool fatnomoredirentry(struct fatdirentry *direntry) {
	return direntry == NULL || direntry->filename[0] == 0x00;
}

bool fatreadsector(fat_entry_t cluster, uint32_t sector, void *data) {
	uint32_t clusterlba = fatclustertolba(cluster);
	// printf("c=%u,s=%u,pos=0x%x\n", cluster, sector, (clusterlba + sector) * SECTOR_SIZE);
	return ataread(clusterlba + sector, 1, data);
}

bool fatwritesector(fat_entry_t cluster, uint32_t sector, const void *data) {
	uint32_t clusterlba = fatclustertolba(cluster);
	return atawrite(clusterlba + sector, 1, data);
}

bool fatcache(uint32_t fat, uint32_t offsetsector) {
	if (fat >= _vbootsector->fatcount || offsetsector >= _vbootsector->fatsize32)
		return false;

	// write back to all fats if dirty
	fatcommit();

	// sectors buffered is either the CACHE_SIZE or the number of sectors until the end of the fat
	_fatcache.sectorsbuffered = umin(FAT_CACHE_SIZE, _vbootsector->fatsize32 - offsetsector);

	uint32_t lba = _vbootsector->reservedsectors + (fat * _vbootsector->fatsize32) + offsetsector;

	ataread(lba, _fatcache.sectorsbuffered, _fatcache.buffer);

	_fatcache.dirty = false;
	_fatcache.firstentry = offsetsector * ENTRIES_PER_SECTOR;
	_fatcache.startingsector = offsetsector;

	return true;
}

void fatcommit(void) {
	if (_fatcache.dirty) {
		// position of cached area in fat 0
		uint32_t lba = _vbootsector->reservedsectors + _fatcache.startingsector;
		for (uint32_t fatidx = 0; fatidx < _vbootsector->fatcount; ++fatidx) {
			atawrite(lba, _fatcache.sectorsbuffered, _fatcache.buffer);
			lba += _vbootsector->fatsize32;
		}
		_fatcache.dirty = false;
	}
}

uint32_t fattotalclusters(fat_entry_t cluster, uint32_t *lastcluster) {
	uint32_t count = 0;
	fat_entry_t value = cluster;
	do {
		if (lastcluster) *lastcluster = value;
		value = fatclustervalue(value);
		count++;
	} while (value < FEEND32L);
	
	return count;
}

fat_entry_t fatclustervalue(uint32_t entryposition) {
	if (entryposition < _fatcache.firstentry || entryposition >= _fatcache.firstentry + (ENTRIES_PER_SECTOR * _fatcache.sectorsbuffered))
		fatcache(0, entryposition / ENTRIES_PER_SECTOR);

	return _fatcache.buffer[entryposition % (ENTRIES_PER_SECTOR * _fatcache.sectorsbuffered)];
}

void fatsetcluster(uint32_t cluster, fat_entry_t value) {
	if (cluster < _fatcache.firstentry || cluster >= _fatcache.firstentry + (ENTRIES_PER_SECTOR * _fatcache.sectorsbuffered))
		fatcache(0, cluster / ENTRIES_PER_SECTOR);

	_fatcache.dirty = true;
	_fatcache.buffer[cluster % (ENTRIES_PER_SECTOR * _fatcache.sectorsbuffered)] = value;
	
	if (value == FEFREE && cluster < _fatsearch.start)
		_fatsearch.start = cluster - 1;
}

uint32_t fatfindfreecluster(void) {

	// start where we are certain there is nothing open behind
	uint32_t current = _fatsearch.start;

	uint32_t possible_entries_left = (_vbootsector->fatsize32 * _vbootsector->bytespersector / sizeof(fat_entry_t)) - current * sizeof(fat_entry_t);

	while (possible_entries_left--) {
		fat_entry_t value = fatclustervalue(current);
		if (value == FEFREE) {
			_fatsearch.start = current + 1;
			return current;
		}
		current++;
	}

	printf("run out of disk space");

	_fatsearch.start = _vbootsector->fatsize32 * _vbootsector->bytespersector / sizeof(fat_entry_t);

	return 0;
}

fat_entry_t fattraverse(fat_entry_t start, uint32_t count) {
	if (count == 0) return start;

	fat_entry_t nextcluster = start;
	while (count--) {
		uint32_t value = fatclustervalue(nextcluster);
		if (value >= FEEND32L)
			return nextcluster;
		nextcluster = value;
	}

	return nextcluster;
}

fat_entry_t fatallocclusters(fat_entry_t start, uint32_t count) {
	if (count == 0) return start;

	uint32_t clustersleft = count;
	fat_entry_t top = start;
	while (clustersleft--) {
		uint32_t freecluster = fatfindfreecluster();
		fatsetcluster(top, freecluster);
		top = freecluster;
	}

	fatsetcluster(top, FEEND32L);

	return top;
}

fat_entry_t fatdeallocclusters(fat_entry_t start) {

	fat_entry_t nextcluster = fatclustervalue(start);

	while (nextcluster < FEEND32L) {
		fat_entry_t value = fatclustervalue(nextcluster);
		fatsetcluster(nextcluster, FEFREE);
		nextcluster = value;
	}

	fatsetcluster(start, FEEND32L);
	return start;

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

int _fatformatfilename(const char *name, char *dest, uint32_t max) {

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

void fatformatfilename(const char *input, uint32_t inputlength, char *output) {

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

	_fatformatfilename(input, output, umin(FAT_FILENAME_LEN, inputlength));

}
