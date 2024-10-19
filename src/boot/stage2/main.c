#include "fat.h"
#include "ata.h"
#include "file.h"
#include "elf.h"
#include "write.h"

extern uint16_t _memregioncount;
extern struct memregion _memregions[];

// to read elf file
bool elfread(struct file *file, void *scratch, void (**entry)());

void stage2main(void) {

	clrscr();

	atainit();
	fatinit();
	fileinit();

#if defined (KERNEL_LOAD) && defined (KERNEL_NAME)

	struct file kernel;
	void (*kernelentry)(); // we ignore the args here

	if (fileopen(&kernel, KERNEL_NAME, 0) == 0)
		return;

	// read kernel header to scratch space
	if (elfread(&kernel, (void *) 0x10000, &kernelentry) == 0)
		return;
    
	kernelentry(atagetidentify(), _memregions, _memregioncount);

#else
	puterr("KERNEL_LOAD OR KERNEL_NAME NOT DEFINED\n", 0);
#endif

}
