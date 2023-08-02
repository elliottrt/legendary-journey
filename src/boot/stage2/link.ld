OUTPUT_FORMAT("binary")
ENTRY(_entry)
SECTIONS
{
    . = 0x7E00;

    .entry : {__entry_start = .; *(.entry)}
    .text : { __text_start = .; *(.text) }
    .data : { __data_start = .; *(.data) }
    end = .;
}