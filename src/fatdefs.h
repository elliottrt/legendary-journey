#ifndef _FATDEFS
#define _FATDEFS

#include "types.h"

#define FAT_FILENAME_LEN 8
#define FAT_FILEEXT_LEN 3
#define FAT_FILETOTAL_LEN (FAT_FILENAME_LEN+FAT_FILEEXT_LEN)
#define SECTOR_SIZE 512

#define FAT_DOT ".          "
#define FAT_DOTDOT "..         "

typedef uint fat_entry_t;
#define ENTRY_SIZE sizeof(fat_entry_t)
#define ENTRIES_PER_SECTOR (SECTOR_SIZE/ENTRY_SIZE)

enum fatdirattributes {
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

struct fatdirentry {
	uchar filename[FAT_FILENAME_LEN];
	uchar fileext[FAT_FILEEXT_LEN];
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

#define FAT_PHYS_ROOT_ENTRY ((struct fatdirentry){ .attributes = SYSTEM | DIRECTORY, .firstclusterlo = _bootsector->rootcluster })
#define FAT_VIRT_ROOT_ENTRY ((struct fatdirentry){ .attributes = SYSTEM | DIRECTORY, .firstclusterlo = _vbootsector->rootcluster })

struct fatbootsector {
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

	// fat32 specific
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

} __attribute__ ((packed));

enum fatentry {
	FEFREE = 0,
	FERESERVED = 1,
	FEDATA = 2,
	FEBAD32 = 0x0FFFFFF7,
	FEEND32L = 0x0FFFFFF8,
	FEEND32H = 0x0FFFFFFF
};

#endif
