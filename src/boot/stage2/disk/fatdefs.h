#ifndef _STAGE2_FATDEFS
#define _STAGE2_FATDEFS

#include <stdint.h>

#define FAT_FILENAME_LEN 8
#define FAT_FILEEXT_LEN 3
#define FAT_FILETOTAL_LEN (FAT_FILENAME_LEN+FAT_FILEEXT_LEN)
/* TODO: this is really bad because we don't know what the sector size is */
#define SECTOR_SIZE 512

#define ENTRY_TYPE uint32_t
#define ENTRY_SIZE sizeof(ENTRY_TYPE)
#define ENTRIES_PER_SECTOR (SECTOR_SIZE/ENTRY_SIZE)

enum fatdirattr
{
	READ_ONLY = 0x01,
	HIDDEN = 0x02,
	SYSTEM = 0x04,
	VOLUME_ID = 0x08,
	DIRECTORY = 0x10,
	ARCHIVE = 0x20,
	DEVICE = 0x40,
	UNUSED = 0x80,
	LFN = READ_ONLY | HIDDEN | SYSTEM | VOLUME_ID 
};

struct fatdirentry
{
	unsigned char file_name[FAT_FILENAME_LEN];
	unsigned char file_ext[FAT_FILEEXT_LEN];
	uint8_t attributes;
	uint8_t reserved;
	uint8_t creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t first_cluster_hi;
    uint16_t modify_time;
    uint16_t modify_date;
    uint16_t first_cluster_lo;
    uint32_t file_size;
} __attribute__ ((packed));

/*
struct fatbootsector
{
	uint8_t boot_jmp[3];
	unsigned char oem_name[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t fat_count;
	uint16_t root_entry_count;
	uint16_t total_sectors_16;
	uint8_t media_type;
	uint16_t fat_size_16;
	uint16_t sectors_per_track;
	uint16_t head_count;
	uint32_t hidden_sectors;
	uint32_t total_sectors_32;

	union __attribute__((packed))
	{
		struct __attribute__((packed))
		{
			uint8_t drive_number;
			uint8_t reserved;
			uint8_t boot_signature;
			uint32_t volume_id;
			unsigned char volume_label[11];
			unsigned char fs_type[8];
			uint8_t boot_code[448];
		} fat1216;
		struct __attribute__((packed))
		{
			uint32_t fat_size_32;
			uint16_t extension_flags;
			uint16_t file_system_version;
			uint32_t root_cluster;
			uint16_t fs_info_sector;
			uint16_t backup_boot_sector;
			uint8_t reserved[12];
			uint8_t drive_number;
			uint8_t reserved1;
			uint8_t boot_signature;
			uint32_t volume_id;
			unsigned char volume_label[11];
			unsigned char fs_type[8];
			uint8_t boot_code[420];
		} fat32;
	} ex;

	uint8_t bootSignature[2];

} __attribute__ ((packed));
*/

struct fatbootsector
{
	uint8_t boot_jmp[3];
	unsigned char oem_name[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t fat_count;
	uint16_t root_entry_count;
	uint16_t total_sectors_16;
	uint8_t media_type;
	uint16_t fat_size_16;
	uint16_t sectors_per_track;
	uint16_t head_count;
	uint32_t hidden_sectors;
	uint32_t total_sectors_32;

	union __attribute__((packed))
	{
		struct __attribute__((packed))
		{
			uint8_t drive_number;
			uint8_t reserved;
			uint8_t boot_signature;
			uint32_t volume_id;
			unsigned char volume_label[11];
			unsigned char fs_type[8];
			uint8_t unused[28];
		} fat1216;
		struct __attribute__((packed))
		{
			uint32_t fat_size_32;
			uint16_t extension_flags;
			uint16_t file_system_version;
			uint32_t root_cluster;
			uint16_t fs_info_sector;
			uint16_t backup_boot_sector;
			uint8_t reserved[12];
			uint8_t drive_number;
			uint8_t reserved1;
			uint8_t boot_signature;
			uint32_t volume_id;
			unsigned char volume_label[11];
			unsigned char fs_type[8];
		} fat32;
	} ex;

} __attribute__ ((packed));

enum fatentry
{
	FEFREE = 0,
	FERESERVED = 1,
	FEDATA = 2,
	FEBAD12 = 0x0FF7,
	FEEND12 = 0x0FFF,
	FEBAD16 = 0xFFF7,
	FEEND16 = 0xFFFF,
	FEBAD32 = 0x0FFFFFF7,
	FEEND32 = 0x0FFFFFF8
};

#endif
