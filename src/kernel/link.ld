OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)

#ifndef KERNBASE
#error KERNBASE must be defined
#endif

SECTIONS
{
	. = KERNBASE + 0x100000;
	
	.text : AT (0x100000) { *(.text .text.*) }

	PROVIDE(etext = .);

	.rodata : { *(.rodata .rodata.*) }

	. = ALIGN(0x1000);

	PROVIDE(data = .);
	
	.data : { *(.data) }
	
	PROVIDE(edata = .);

	.bss : { *(.bss) }
	
	PROVIDE(end = .);

}