#ifndef _STAGE2_FAT
#define _STAGE2_FAT

#include "fatdefs.h"

extern struct fatbootsector *_fboot;

void fat_init(void *boot_sector_address);

ENTRY_TYPE fat_direntry_cluster(struct fatdirentry *direntry);
int fat_free_direntry(struct fatdirentry *direntry);
int fat_no_more_direntry(struct fatdirentry *direntry);
void fat_read_cluster(ENTRY_TYPE cluster, void *out);
void fat_read_sector(ENTRY_TYPE cluster, uint32_t sector, void *out);

int32_t fat_cache(uint32_t fat, uint32_t offset_sector);
uint32_t fat_total_clusters(ENTRY_TYPE cluster);
ENTRY_TYPE fat_cluster_value(uint32_t entry_position);

void fat_format_filename(const char *input, uint32_t input_length, char *output);

#endif
