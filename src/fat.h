#ifndef _FAT
#define _FAT

#include "fatdefs.h"
#include "types.h"

#define FAT_DEFAULT_FILENAME_CHAR '_'
#define FAT_DEFAULT_FILENAME_PADDING ' '
#define FAT_COUNT 2

#define FAT_CACHE_SIZE 1

#define _bootsector ((struct fatbootsector *) 0x7C00)
#define _vbootsector ((struct fatbootsector *) V2P_WO(0x7C00))

void fatinit(void);

int fatnomoredirentry(struct fatdirentry *direntry);
void fatreadsector(ENTRY_TYPE cluster, uint sector, void *out);

int fatcache(uint fat, uint offsetsector);
uint fattotalclusters(ENTRY_TYPE cluster);
ENTRY_TYPE fatclustervalue(uint entryposition);

void fatformatfilename(const char *input, uint inputlength, char *output);

#endif
