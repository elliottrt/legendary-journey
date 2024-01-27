#include "ata.h"
#include "x86.h"
#include "write.h"

// UNUSED
void atacacheflush(void);
void atareset(void);
int atawrite(uint lba, uchar sectors, const void *src);
uint atasectors(void);

struct ataidentify _ataidentify;

void atainit(void) {
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
		puterr("\nata: error: drive does not exist\n", 0);
		return;
	}
	// wait for BSY (bit 7) to clear
	while (inb(0x01F7) & 0x80);
	// ensure drive is ATA
	if (inb(0x01F4) != 0 || inb(0x01F5) != 0)
	{
		puterr("\nata: error: drive is not ata", 0);
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
		puterr("\nata: error: lba28 not supported", 0);

}

int ataread(uint lba, uchar sectors, void *dst) {
	uchar status = 0x00;
	ushort *data = (ushort *) dst;

	lba &= 0x0FFFFFFF;

	if (lba + sectors > _ataidentify.useraddressablesectors) {
		puterr("ata: error: cannot read beyond end of disk\n", 1);
		return -1;
	}

	/* why do io if we aren't reading anything? */
	/* this is also a special value that would read a lot of sectors */
	if (sectors == 0) return 0;

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

	if (atacheckerror()) {
		ataerror();
		return -1;
	}
		
	return 0;

}

int atacheckerror(void) {
	/* read status port */
	uchar status = inb(0x01F7);
	/* return whether ERR bit is set */
	return (status & 1) != 0;
}

void ataerror(void) {
	/* read error register */
	uchar error = inb(0x01F1);
	if (error & BIT(0))
		puterr("\nata: error: address mark not found\n", 0);
	if (error & BIT(1))
		puterr("\nata: error: track zero not found\n", 0);
	if (error & BIT(2))
		puterr("\nata: error: aborted command\n", 1);
	if (error & BIT(3))
		puterr("\nata: error: media change request\n", 1);
	if (error & BIT(4))
		puterr("\nata: error: id not found\n", 0);
	if (error & BIT(5))
		puterr("\nata: error: media changed\n", 1);
	if (error & BIT(6))
		puterr("\nata: error: uncorrectable data error\n", 0);
	if (error & BIT(7))
		puterr("\nata: error: bad block detected\n", 1);
}

struct ataidentify *atagetidentify(void) {
	return &_ataidentify;
}
