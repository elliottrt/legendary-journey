#ifndef _FAT
#define _FAT

#include "fatdefs.h"
#include "types.h"
#include "kernel/mmu.h"

#define FAT_DEFAULT_FILENAME_CHAR '_'
#define FAT_DEFAULT_FILENAME_PADDING ' '
#define FAT_COUNT 2

#define FAT_CACHE_SIZE 1

#define _bootsector ((struct fatbootsector *) 0x7C00)
#define _vbootsector ((struct fatbootsector *) V2P_WO(0x7C00))

void fatinit(void);

uint fatclustertolba(fat_entry_t cluster);
int fatnomoredirentry(struct fatdirentry *direntry);
int fatreadsector(fat_entry_t cluster, uint sector, void *data);
int fatwritesector(fat_entry_t cluster, uint sector, const void *data);

int fatcache(uint fat, uint offsetsector);
void fatcommit(void);
uint fattotalclusters(fat_entry_t cluster, uint *lastcluster);
fat_entry_t fatclustervalue(uint entryposition);

uint fatfindfreecluster(void);
void fatsetcluster(uint cluster, fat_entry_t value);

// returns the new end cluster
fat_entry_t fatallocclusters(fat_entry_t start, uint count);

void fatformatfilename(const char *input, uint inputlength, char *output);

#endif
