OUTPUT_FORMAT("binary")
ENTRY(_entry)
SECTIONS
{
    . = 0x500;

    .entry : {__entry_start = .; *(.entry)}
    .text : { __text_start = .; *(.text) }
    .data : { __data_start = .; *(.data) }
    end = .;
}