#ifndef _FATDEFS
#define _FATDEFS

#include "types.h"

#define FAT_FILENAME_LEN 8
#define FAT_FILEEXT_LEN 3
#define FAT_FILETOTAL_LEN (FAT_FILENAME_LEN+FAT_FILEEXT_LEN)
/* TODO: this is bad because we don't know what the sector size is */
#define SECTOR_SIZE 512

#define ENTRY_TYPE uint
#define ENTRY_SIZE sizeof(ENTRY_TYPE)
#define ENTRIES_PER_SECTOR (SECTOR_SIZE/ENTRY_SIZE)

enum fatdirattributes
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
	unsigned char filename[FAT_FILENAME_LEN];
	unsigned char fileext[FAT_FILEEXT_LEN];
	uchar attributes;
	uchar reserved;
	uchar creationtimetenths;
    ushort creationtime;
    ushort creationdate;
    ushort accessdate;
    ushort firstclusterhi;
    ushort modifytime;
    ushort modifydate;
    ushort firstclusterlo;
    uint filesize;
} __attribute__ ((packed));

struct fatbootsector
{
	uchar bootjmp[3];
	unsigned char oemname[8];
	ushort bytespersector;
	uchar sectorspercluster;
	ushort reservedsectors;
	uchar fatcount;
	ushort rootentrycount;
	ushort totalsectors16;
	uchar mediatype;
	ushort fatsize16;
	ushort sectorspertrack;
	ushort headcount;
	uint hiddensectors;
	uint totalsectors32;

	union __attribute__((packed))
	{
		struct __attribute__((packed))
		{
			uchar drivenumber;
			uchar reserved;
			uchar bootsignature;
			uint volumeid;
			unsigned char volumelabel[11];
			unsigned char fstype[8];
			uchar unused[28];
		} fat1216;
		struct __attribute__((packed))
		{
			uint fatsize32;
			ushort extensionflags;
			ushort filesystemversion;
			uint rootcluster;
			ushort fsinfosector;
			ushort backupbootsector;
			uchar reserved[12];
			uchar drivenumber;
			uchar reserved1;
			uchar bootsignature;
			uint volumeid;
			uchar volumelabel[11];
			uchar fstype[8];
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
