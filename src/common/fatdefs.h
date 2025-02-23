#ifndef _FATDEFS
#define _FATDEFS

#include "types.h"

#define FAT_FILENAME_LEN 8
#define FAT_FILEEXT_LEN 3
#define FAT_FILETOTAL_LEN (FAT_FILENAME_LEN+FAT_FILEEXT_LEN)
#define SECTOR_SIZE 512

#define FAT_DOT ".          "
#define FAT_DOTDOT "..         "

typedef uint32_t fat_entry_t;
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
	uint8_t filename[FAT_FILENAME_LEN];
	uint8_t fileext[FAT_FILEEXT_LEN];
	uint8_t attributes;
	uint8_t reserved;
	uint8_t creationtimetenths;
    uint16_t creationtime;
    uint16_t creationdate;
    uint16_t accessdate;
    uint16_t firstclusterhi;
    uint16_t modifytime;
    uint16_t modifydate;
    uint16_t firstclusterlo;
    uint32_t filesize;
} __attribute__ ((packed));

#define FAT_PHYS_ROOT_ENTRY ((struct fatdirentry){ .attributes = SYSTEM | DIRECTORY, .firstclusterlo = _bootsector->rootcluster })
#define FAT_VIRT_ROOT_ENTRY ((struct fatdirentry){ .attributes = SYSTEM | DIRECTORY, .firstclusterlo = _vbootsector->rootcluster })

struct fatbootsector {
	uint8_t bootjmp[3];
	unsigned char oemname[8];
	uint16_t bytespersector;
	uint8_t sectorspercluster;
	uint16_t reservedsectors;
	uint8_t fatcount;
	uint16_t rootentrycount;
	uint16_t totalsectors16;
	uint8_t mediatype;
	uint16_t fatsize16;
	uint16_t sectorspertrack;
	uint16_t headcount;
	uint32_t hiddensectors;
	uint32_t totalsectors32;

	// fat32 specific
	uint32_t fatsize32;
	uint16_t extensionflags;
	uint16_t filesystemversion;
	uint32_t rootcluster;
	uint16_t fsinfosector;
	uint16_t backupbootsector;
	uint8_t reserved[12];
	uint8_t drivenumber;
	uint8_t reserved1;
	uint8_t bootsignature;
	uint32_t volumeid;
	uint8_t volumelabel[11];
	uint8_t fstype[8];

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
