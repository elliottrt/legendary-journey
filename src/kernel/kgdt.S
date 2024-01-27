.code32

.globl kgdtinit

.section .text

kgdtinit:
    pushl %ebp
    movl %esp, %ebp

    lgdt (kgdt_desc)

    movl %ebp, %esp
    popl %ebp
    ret

.section .data

kgdt_start:
kgdt_null:
    .quad 0
kgdt_code:
    .word 0xFFFF       // limit lo
    .word 0x0000       // base lo
    .byte 0x00         // base mid
    .byte 0b10011010   // access
    .byte 0b11001111   // granularity
    .byte 0x00         // base hi
kgdt_data:
    .word 0xFFFF       // limit lo
    .word 0x0000       // base lo
    .byte 0x00         // base mid
    .byte 0b10010010   // access
    .byte 0b11001111   // granularity
    .byte 0x00         // base hi
kgdt_ucode:
    .word 0xFFFF       // limit lo
    .word 0x0000       // base lo
    .byte 0x00         // base mid
    .byte 0b11111010   // access
    .byte 0b11001111   // granularity
    .byte 0x00         // base hi
kgdt_udata:
    .word 0xFFFF       // limit lo
    .word 0x0000       // base lo
    .byte 0x00         // base mid
    .byte 0b11110010   // access
    .byte 0b11001111   // granularity
    .byte 0x00         // base hi
kgdt_tss:
    .quad 0
kgdt_end:

kgdt_desc:
    .word kgdt_desc - kgdt_start - 1
	.long kgdt_start

