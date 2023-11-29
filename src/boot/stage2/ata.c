#include "ata.h"
#include "x86.h"
#include "write.h"

struct ataidentify _ataidentify;

void atainit(void)
{
	/* avoid unaligned pointer warning */
	void *tempaddr = (void *) &_ataidentify;
	ushort *ataidentifyaddress = (ushort *) tempaddr;

	/* select master drive */
	outb(0x01F6, 0xA0);
	/* zero other ports */
	outb(0x01F2, 0x00);
	outb(0x01F3, 0x00);
	outb(0x01F4, 0x00);
	outb(0x01F5, 0x00);
	/* send IDENTIFY command */
	outb(0x01F7, 0xEC);
	/* read status */
	uchar status = inb(0x1F7);
	/* if status is 0 there is no drive */
	if (status == 0)
	{
		puterr("\nATA: Error: Drive does not exist\n", 0);
		return;
	}
	// wait for BSY (bit 7) to clear
	while (inb(0x01F7) & 0x80);
	// ensure drive is ATA
	if (inb(0x01F4) != 0 || inb(0x01F5) != 0)
	{
		puterr("\nATA: Error: Drive is not ATA", 0);
		return;
	}
	/* wait for error or ready for data */
	status = 0x00;
	while ((status & 0x09) == 0)
		status = inb(0x01F7);
	/* if error, print and return failure */
	if (status & 1)
	{
		ataerror();
		return;
	}
	/* otherwise we are ready to read data */
	for (int reads = 0; reads < 256; reads++)
	{
		ataidentifyaddress[reads] = inw(0x01F0);
	}

	/* make sure our lba28 is supported */
	if (_ataidentify.useraddressablesectors == 0)
		puterr("\nATA: Error: LBA28 not supported", 0);

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

	if (lba + sectors > _ataidentify.useraddressablesectors)
		puterr("ATA: Error: Cannot read beyond end of disk\n", 1);

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

	/* we don't want to read past the end of the disk */
	if (lba + sectors > _ataidentify.useraddressablesectors)
		puterr("ATA: Error: Cannot write beyond end of disk\n", 1);

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
	if (error & BIT(0))
		puterr("\nATA: Error: Address mark not found\n", 0);
	if (error & BIT(1))
		puterr("\nATA: Error: Track zero not found\n", 0);
	if (error & BIT(2))
		puterr("\nATA: Error: Aborted command\n", 1);
	if (error & BIT(3))
		puterr("\nATA: Error: Media change request\n", 1);
	if (error & BIT(4))
		puterr("\nATA: Error: ID not found\n", 0);
	if (error & BIT(5))
		puterr("\nATA: Error: Media changed\n", 1);
	if (error & BIT(6))
		puterr("\nATA: Error: Uncorrectable data error\n", 0);
	if (error & BIT(7))
		puterr("\nATA: Error: Bad block detected\n", 1);
}

uint atasectors(void) 
{
	return _ataidentify.useraddressablesectors;
}
