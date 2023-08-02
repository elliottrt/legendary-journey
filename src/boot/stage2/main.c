
#include <stdint.h>

#include "write.h"
#include "ata.h"
#include "std.h"
#include "memory.h"
#include "rsdp.h"

void stage2main(void)
{

    clrscr();
    puts("Operating System\n");

	mem_init();
	ata_init();
	rsdp_init();

	/*ata_read(0, 1, boot);*/

}
