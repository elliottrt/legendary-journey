OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)

SECTIONS
{
	. = 0x100000;
	.text : { __text_start = .; *(.text) }
	.data : { __data_start = .; *(.data) }
	.bss : { __bss_start = .; *(.bss) }
	PROVIDE(end = .);
}
