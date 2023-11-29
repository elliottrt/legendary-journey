#include "types.h"
#include "printf.h"
#include "ata.h"

// end of kernel memory
extern char end[];

void main(void)
{

	printf("Loaded %d bytes of Kernel!\n", end - 0x100000);

	atainit();

	while(1);
}
