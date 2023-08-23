
#include <stdint.h>

#include "write.h"
#include "disk/fat.h"
#include "disk/ata.h"
#include "disk/pfile.h"
#include "std.h"
#include "memory.h"

#define READ 1024

void stage2main(void)
{

	struct pfile file;
	char buffer[READ];

    clrscr();
    puts("Operating System\n");

	/* mem_init(); */
	ata_init();
	fat_init((void *) 0x7C00);
	pfile_init();


	if (open(&file, "/test.txt") < 0)
	{
		puterr("couldn't open file", 1);
		puth(errno);
	}
	else
	{
		read(&file, buffer, READ);
		putsn(buffer, READ); 
	}

	return;

}
