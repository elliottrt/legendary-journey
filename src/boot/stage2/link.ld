OUTPUT_FORMAT("binary")
ENTRY(_entry)
SECTIONS
{
    . = 0x7E00;

    .text.prologue : { *(.text.prologue) }

    .rodata : { *(.rodata) }

    .text : { *(.text) }

    .data : { *(.data) }

    .bss : { *(.bss) }

    end = .;

    /DELETE/ : { *(.comment) }
}