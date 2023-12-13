#include "fat.h"
#include "ata.h"
#include "file.h"
#include "elf.h"
#include "write.h"

void stage2main(void)
{

	clrscr();

	atainit();
	fatinit((void *) 0x7C00);
	fileinit();

#if defined (KERNEL_LOAD) && defined (KERNEL_NAME)

	struct file kernel;
	void (*kernelentry)(uint);

	if (fileopen(&kernel, KERNEL_NAME) < 0)
		return;

	// read kernel header to scratch space
	if (elfread(&kernel, (void *) 0x10000, &kernelentry) < 0)
		return;
    
	kernelentry(atasectors());

#else
	puterr("KERNEL_LOAD OR KERNEL_NAME NOT DEFINED\n", 0);
#endif

}
