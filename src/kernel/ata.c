#include "ata.h"
#include "x86.h"
#include "printf.h"

extern uint disksectors;

void atainit(void)
{
	printf("ATA: Initialized, %d sectors available\n", disksectors);
}

void atacacheflush(void)
{
	/* send cache flush byte to command port */
	outb(0x01F0, 0xE7);
	/* wait for BSY (bit 7) to clear */
	while (inb(0x01F7) & 0x80);

	if (atacheckerror())
		ataerror();
}

void atareset(void)
{
	/* get current control values */
	uchar control = inb(0x03F6);
	/* send it back with software reset bit set */
	outb(0x03F6, control | 0x04);
	/* set it back to off */
	outb(0x03F6, control);

	if (atacheckerror())
		ataerror();
}

void ataread(uint lba, uchar sectors, void *dst)
{
	uchar status = 0x00;
	ushort *data = (ushort *) dst;

	lba &= 0x0FFFFFFF;

	if (lba + sectors > disksectors) 
	{
		printferr();
		printf("ATA: Error: Cannot read beyond end of disk\n");
		printfstd();
	}

	/* why do io if we aren't reading anything? */
	/* this is also a special value that would read a lot of sectors */
	if (sectors == 0) return;

	/* drive and lba bits 24-27 */
	outb(0x01F6, (lba >> 24) | 0xE0);
	/* sectors to read */
	outb(0x01F2, sectors);
	/* lba bits 0-7 */
	outb(0x01F3, lba);
	/* lba bits 8-15 */
	outb(0x01F4, lba >> 8);
	/* lba bits 16-23 */
	outb(0x01F5, lba >> 16);
	/* command: read w/ retry */
	outb(0x01F7, 0x20);

	/* wait until ready to read */
	while ((status & 8) == 0)
		status = inb(0x01F7);

	for (int reads = 0; reads < sectors * 256; reads++)	
	{
		data[reads] = inw(0x01F0);
	}

	if (atacheckerror())
		ataerror();

}

void atawrite(uint lba, uchar sectors, const void *src)
{

	uchar status = 0x00;
	const ushort *data = (ushort *) src;

	lba &= 0x0FFFFFFF;

	if (lba + sectors > disksectors)
	{
		printferr();
		printf("ATA: Error: Cannot write beyond end of disk\n");
		printfstd();
	}

	/* why do io if we aren't writing anything? */
	/* this is also a special value that would write a lot of sectors */
	if (sectors == 0) return;

	/* drive and lba bits 24-27 */
	outb(0x01F6, (lba >> 24) | 0xE0);
	/* sectors to write */
	outb(0x01F2, sectors);
	/* lba bits 0-7 */
	outb(0x01F3, lba);
	/* lba bits 8-15 */
	outb(0x01F4, lba >> 8);
	/* lba bits 16-23 */
	outb(0x01F5, lba >> 16);
	/* command: write w/ retry */
	outb(0x01F7, 0x30);

	/* wait until ready to write */
	while ((status & 8) == 0)
		status = inb(0x01F7);

	for (int writes = 0; writes < sectors * 256; writes++)	
	{
		outw(0x01F0, data[writes]);
	}

	if (atacheckerror())
		ataerror();

	/* need to do this to prevent issues */
	atacacheflush();

}

int atacheckerror(void)
{
	/* read status port */
	uchar status = inb(0x01F7);
	/* return whether ERR bit is set */
	return (status & 1) != 0;
}

void ataerror(void)
{
	/* read error register */
	uchar error = inb(0x01F1);

	printferr();

	if (error & BIT(0))
		printf("\nATA: Error: Address mark not found\n");
	if (error & BIT(1))
		printf("\nATA: Error: Track zero not found\n");
	if (error & BIT(2))
		printf("\nATA: Error: Aborted command\n");
	if (error & BIT(3))
		printf("\nATA: Error: Media change request\n");
	if (error & BIT(4))
		printf("\nATA: Error: ID not found\n");
	if (error & BIT(5))
		printf("\nATA: Error: Media changed\n");
	if (error & BIT(6))
		printf("\nATA: Error: Uncorrectable data error\n");
	if (error & BIT(7))
		printf("\nATA: Error: Bad block detected\n");

	printfstd();
}

uint atasectors(void)
{
	return disksectors;
}
