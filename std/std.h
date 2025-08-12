#ifndef LEGENDARY_JOURNEY_STDLIB
#define LEGENDARY_JOURNEY_STDLIB

// TODO: documentation
// TODO: don't expose struct fatdirentry or struct file, use opaque pointers instead

#define NULL ((void*)0)

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef int32_t ssize_t;
typedef uint32_t size_t;
typedef uint32_t uintptr_t;

_Static_assert(sizeof(int8_t) == 1, "int8_t must be 1 byte");
_Static_assert(sizeof(uint8_t) == 1, "uint8_t must be 1 byte");
_Static_assert(sizeof(int16_t) == 2, "int16_t must be 2 bytes");
_Static_assert(sizeof(uint16_t) == 2, "uint16_t must be 2 bytes");
_Static_assert(sizeof(int32_t) == 4, "int32_t must be 4 bytes");
_Static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");
_Static_assert(sizeof(int64_t) == 8, "int64_t must be 8 bytes");
_Static_assert(sizeof(uint64_t) == 8, "uint64_t must be 8 bytes");

_Static_assert(sizeof(uintptr_t) == sizeof(void *), "uintptr_t must the same size of a void *");

typedef unsigned int bool;
#define true (1)
#define false (0)

#define FAT_FILENAME_LEN 8
#define FAT_FILEEXT_LEN 3

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

struct file {
	
	struct fatdirentry fsentry;

	bool opened;
	bool dirty;

	uint32_t position;
	uint8_t buffer[512];

	uint32_t totalclusters;
	uint32_t firstcluster;
	uint32_t lastcluster;
    uint32_t currentcluster;
	uint32_t parentdircluster;
    uint32_t sectorincluster;

};

enum fflags {
	FTRUNC = 1 << 0, /* open file at start */
	FAPPEND = 1 << 1, /* open file at end */
	FCREATE = 1 << 2, /* create a file if none exists */
	FDIRECTORY = 1 << 3, /* open a dir, or create a dir if FCREATE set */
};

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

extern void printf(const char *format, ...);
extern void putc(char ch);
extern void puts(const char *str);

extern uint64_t timer_read(void);

extern bool fileopen(struct file *file, const char *pathname, int flags);

extern int32_t fileread(struct file *file, void *buffer, uint32_t size);
extern int32_t filewrite(struct file *file, const void *buffer, uint32_t size);

extern uint32_t filesize(struct file *file);
extern bool fileresize(struct file *file, uint32_t size);

extern bool fileseek(struct file *file, uint32_t seek);
extern uint32_t filetell(struct file *file);

extern bool fileflush(struct file *file);
extern bool fileclose(struct file *file);

extern void *malloc(size_t size);
extern void free(void *ptr);

#endif
