#ifndef _STAGE2_FAT
#define _STAGE2_FAT

#include "fatdefs.h"
#include "types.h"

#define FAT_DEFAULT_FILENAME_CHAR '_'
#define FAT_DEFAULT_FILENAME_PADDING ' '
#define FAT_COUNT 2

extern struct fatbootsector *_bootsector;

void fatinit(void *bootsectoraddress);

int fatnomoredirentry(struct fatdirentry *direntry);
void fatreadsector(ENTRY_TYPE cluster, uint sector, void *out);

int fatcache(uint fat, uint offsetsector);
uint fattotalclusters(ENTRY_TYPE cluster);
ENTRY_TYPE fatclustervalue(uint entryposition);

void fatformatfilename(const char *input, uint inputlength, char *output);

#endif
