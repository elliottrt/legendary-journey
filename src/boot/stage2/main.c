
#include <stdint.h>

#include "write.h"
#include "ata.h"
#include "std.h"

void stage2main(void)
{

    clrscr();
    puts("Operating System\n");

	ata_init();

	/*ata_read(0, 1, boot);*/

}
