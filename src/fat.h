#ifndef _FAT
#define _FAT

#include "fatdefs.h"
#include "types.h"
#include "kernel/memory/mmu.h"

#define FAT_DEFAULT_FILENAME_CHAR '_'
#define FAT_DEFAULT_FILENAME_PADDING ' '

// ignored in stage2
#define FAT_CACHE_SIZE 8

#define _bootsector ((struct fatbootsector *) 0x7C00)
#define _vbootsector ((struct fatbootsector *) P2V_WO(0x7C00))

void fatinit(void);

uint32_t fatclustertolba(fat_entry_t cluster);
bool fatnomoredirentry(struct fatdirentry *direntry);
bool fatreadsector(fat_entry_t cluster, uint32_t sector, void *data);
bool fatwritesector(fat_entry_t cluster, uint32_t sector, const void *data);

bool fatcache(uint32_t fat, uint32_t offsetsector);
void fatcommit(void);
uint32_t fattotalclusters(fat_entry_t cluster, uint32_t *lastcluster);
fat_entry_t fatclustervalue(uint32_t entryposition);

uint32_t fatfindfreecluster(void);
void fatsetcluster(uint32_t cluster, fat_entry_t value);

// travel count clusters from start
fat_entry_t fattraverse(fat_entry_t start, uint32_t count);

// returns the new end cluster
fat_entry_t fatallocclusters(fat_entry_t start, uint32_t count);

// returns the new end cluster
// sets start as the end of a chain and deallocates all after it
fat_entry_t fatdeallocclusters(fat_entry_t start);

void fatformatfilename(const char *input, uint32_t inputlength, char *output);

#endif
