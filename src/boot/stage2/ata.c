#include "ata.h"
#include "asm_call.h"
#include "write.h"

struct ata_identify _ata_identify;

uint32_t ata_init(void)
{
	uint8_t status;
	uint16_t reads;
	uint16_t *data_addr = (uint16_t *) &_ata_identify;

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
	status = inb(0x1F7);
	/* if status is 0 there is no drive */
	if (status == 0)
	{
		puterr("\nATA: Error: Drive does not exist\n", 0);
		return 0;
	}
	/* wait for BSY (bit 7) to clear */
	while (inb(0x01F7) & 0x80);
	/* ensure drive is ATA */
	if (inb(0x01F4) != 0 || inb(0x01F5) != 0)
	{
		puterr("\nATA: Error: Drive is not ATA", 0);
		return 0;
	}
	/* wait for error or ready for data */
	status = 0x00;
	while ((status & 0x09) == 0)
		status = inb(0x01F7);
	/* if error, print and return failure */
	if (status & 1)
	{
		ata_error();
		return 0;
	}
	/* otherwise we are ready to read data */
	for (reads = 0; reads < 256; reads++)
	{
		data_addr[reads] = inw(0x01F0);
	}

	/* make sure our lba28 is supported */
	if (_ata_identify.user_addressable_sectors == 0)
	{
		puterr("\nATA: Error: LBA28 not supported", 0);
	}

	return _ata_identify.user_addressable_sectors;

}

void ata_cache_flush(void)
{
	/* send cache flush byte to command port */
	outb(0x01F0, 0xE7);
	/* wait for BSY (bit 7) to clear */
	while (inb(0x01F7) & 0x80);

	if (ata_check_error())
		ata_error();
}

void ata_reset(void)
{
	/* get current control values */
	uint8_t control = inb(0x03F6);
	/* send it back with software reset bit set */
	outb(0x03F6, control | 0x04);
	/* set it back to off */
	outb(0x03F6, control);

	if (ata_check_error())
		ata_error();
}

void ata_read(uint32_t lba, uint8_t sectors, void *dst)
{
	uint8_t status = 0x00;
	uint16_t reads;
	uint16_t *data = (uint16_t *) dst;

	lba &= 0x0FFFFFFF;

	if (lba + sectors >= _ata_identify.user_addressable_sectors)
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

	for (reads = 0; reads < sectors * 256; reads++)	
	{
		data[reads] = inw(0x01F0);
	}

	if (ata_check_error())
		ata_error();

}

void ata_write(uint32_t lba, uint8_t sectors, void *src)
{

	uint8_t status = 0x00;
	uint16_t writes;
	uint16_t *data = (uint16_t *) src;

	lba &= 0x0FFFFFFF;

	/* we don't want to read past the end of the disk */
	if (lba + sectors >= _ata_identify.user_addressable_sectors)
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

	for (writes = 0; writes < sectors * 256; writes++)	
	{
		outw(0x01F0, data[writes]);
	}

	if (ata_check_error())
		ata_error();

	/* need to do this to prevent issues */
	ata_cache_flush();

}

int ata_check_error(void)
{
	/* read status port */
	uint8_t status = inb(0x01F7);
	/* return whether ERR bit is set */
	return (status & 1) != 0;
}

#define BIT(n) (1 << (n))
void ata_error(void)
{
	/* read error register */
	uint8_t error = inb(0x01F1);
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
