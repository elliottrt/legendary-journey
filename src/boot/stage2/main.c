
#include <stdint.h>

#include "write.h"
#include "ata.h"

void stage2main(void)
{

	uint32_t test[128];

    clrscr();
    puts("Hello, World!\nWelcome to my project!\n");

	puts("Total Sectors: ");
	putu(ata_init());
	putc('\n');

}
