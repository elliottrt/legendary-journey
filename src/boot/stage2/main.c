#include "fat.h"
#include "ata.h"
#include "file.h"
#include "elf.h"
#include "write.h"
#include "mem.h"

void stage2main(void) {

	clrscr();

	atainit();
	fatinit();
	fileinit();

#if defined (KERNEL_LOAD) && defined (KERNEL_NAME)

	struct file kernel;
	void (*kernelentry)(); // we can ignore the args here

	if (fileopen(&kernel, KERNEL_NAME, 0) == 0)
		return;

	// read kernel header to scratch space
	if (elfread(&kernel, (void *) 0x10000, &kernelentry) == 0)
		return;
    
	// TODO: instead of membound, send pointer to memory map
	kernelentry(atagetidentify(), membound());

#else
	puterr("KERNEL_LOAD OR KERNEL_NAME NOT DEFINED\n", 0);
#endif

}
