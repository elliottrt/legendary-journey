.code32

.globl outb
outb:
    movw 4(%esp), %dx
    movb 8(%esp), %al
    outb %al, %dx
    ret

.globl inb
inb:
    movw 4(%esp), %dx
    xorl %eax, %eax
    inb %dx, %al
    ret

.globl outw
outw:
    movw 4(%esp), %dx
    movw 8(%esp), %ax
    outw %ax, %dx
    ret

.globl inw
inw:
    movw 4(%esp), %dx
    xorl %eax, %eax
    inw %dx, %ax
    ret

.globl ltr
ltr:
    pushl %ebp
    movl %esp, %ebp

    movw 8(%esp), %ax
    ltr %ax

    movl %ebp, %esp
    popl %ebp
    ret

.extern gdtdesc
.globl gdtload
gdtload:
    pushl %ebp
    movl %esp, %ebp

    lgdt (gdtdesc)

    movl %ebp, %esp
    popl %ebp
    ret

/*
.section .data

.globl gdt_start
gdt_start:
gdt_null:
    .quad 0
gdt_code:
    .word 0xFFFF       // limit lo
    .word 0x0000       // base lo
    .byte 0x00         // base mid
    .byte 0b10011010   // access
    .byte 0b11001111   // granularity
    .byte 0x00         // base hi
gdt_data:
    .word 0xFFFF       // limit lo
    .word 0x0000       // base lo
    .byte 0x00         // base mid
    .byte 0b10010010   // access
    .byte 0b11001111   // granularity
    .byte 0x00         // base hi
gdt_ucode:
    .word 0xFFFF       // limit lo
    .word 0x0000       // base lo
    .byte 0x00         // base mid
    .byte 0b11111010   // access
    .byte 0b11001111   // granularity
    .byte 0x00         // base hi
gdt_udata:
    .word 0xFFFF       // limit lo
    .word 0x0000       // base lo
    .byte 0x00         // base mid
    .byte 0b11110010   // access
    .byte 0b11001111   // granularity
    .byte 0x00         // base hi
gdt_tss:
    .quad 0
gdt_end:

.globl gdt_desc
gdt_desc:
    .word gdt_desc - gdt_start - 1
	.long gdt_start
*/

// for virtmem.c
.globl lcr3
lcr3:
    pushl %ebp
    movl %esp, %ebp

    movl 8(%esp), %eax
    movl %eax, %cr3

    movl %ebp, %esp
    popl %ebp
    ret

// For idt.c
.globl idtload
idtload:
    pushl %ebp
    movl %esp, %ebp

	movl 8(%ebp), %eax
	lidt (%eax)
    
    movl %ebp, %esp
    popl %ebp

	ret

.globl isrhandler
isrcommon:
    pusha

    xorl %eax, %eax
    movw %ds, %ax
    pushl %eax

    movw $0x8, %ax // GDT CODE_SEG
    movw %ax, %ds
    movw %ax, %es

    pushl %esp
    call isrhandler
    addl $4, %esp

    popl %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    popa
    addl $8, %esp
    iret

.macro ISR_NO_ERR idx
.globl _ISR\idx
_ISR\idx:
    cli
    pushl $0
    pushl $\idx
    jmp isrcommon
.endm

.macro ISR_ERR idx
.globl _ISR\idx
_ISR\idx:
    cli
    pushl $\idx
    jmp isrcommon
.endm

.align 4

ISR_NO_ERR 0
ISR_NO_ERR 1
ISR_NO_ERR 2
ISR_NO_ERR 3
ISR_NO_ERR 4
ISR_NO_ERR 5
ISR_NO_ERR 6
ISR_NO_ERR 7
ISR_ERR 8
ISR_NO_ERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NO_ERR 15
ISR_NO_ERR 16
ISR_ERR 17
ISR_NO_ERR 18
ISR_NO_ERR 19
ISR_NO_ERR 20
ISR_ERR 21
ISR_NO_ERR 22
ISR_NO_ERR 23
ISR_NO_ERR 24
ISR_NO_ERR 25
ISR_NO_ERR 26
ISR_NO_ERR 27
ISR_NO_ERR 28
ISR_NO_ERR 29
ISR_NO_ERR 30
ISR_NO_ERR 31
ISR_NO_ERR 32
ISR_NO_ERR 33
ISR_NO_ERR 34
ISR_NO_ERR 35
ISR_NO_ERR 36
ISR_NO_ERR 37
ISR_NO_ERR 38
ISR_NO_ERR 39
ISR_NO_ERR 40
ISR_NO_ERR 41
ISR_NO_ERR 42
ISR_NO_ERR 43
ISR_NO_ERR 44
ISR_NO_ERR 45
ISR_NO_ERR 46
ISR_NO_ERR 47
ISR_NO_ERR 48
ISR_NO_ERR 49
ISR_NO_ERR 50
ISR_NO_ERR 51
ISR_NO_ERR 52
ISR_NO_ERR 53
ISR_NO_ERR 54
ISR_NO_ERR 55
ISR_NO_ERR 56
ISR_NO_ERR 57
ISR_NO_ERR 58
ISR_NO_ERR 59
ISR_NO_ERR 60
ISR_NO_ERR 61
ISR_NO_ERR 62
ISR_NO_ERR 63
ISR_NO_ERR 64
ISR_NO_ERR 65
ISR_NO_ERR 66
ISR_NO_ERR 67
ISR_NO_ERR 68
ISR_NO_ERR 69
ISR_NO_ERR 70
ISR_NO_ERR 71
ISR_NO_ERR 72
ISR_NO_ERR 73
ISR_NO_ERR 74
ISR_NO_ERR 75
ISR_NO_ERR 76
ISR_NO_ERR 77
ISR_NO_ERR 78
ISR_NO_ERR 79
ISR_NO_ERR 80
ISR_NO_ERR 81
ISR_NO_ERR 82
ISR_NO_ERR 83
ISR_NO_ERR 84
ISR_NO_ERR 85
ISR_NO_ERR 86
ISR_NO_ERR 87
ISR_NO_ERR 88
ISR_NO_ERR 89
ISR_NO_ERR 90
ISR_NO_ERR 91
ISR_NO_ERR 92
ISR_NO_ERR 93
ISR_NO_ERR 94
ISR_NO_ERR 95
ISR_NO_ERR 96
ISR_NO_ERR 97
ISR_NO_ERR 98
ISR_NO_ERR 99
ISR_NO_ERR 100
ISR_NO_ERR 101
ISR_NO_ERR 102
ISR_NO_ERR 103
ISR_NO_ERR 104
ISR_NO_ERR 105
ISR_NO_ERR 106
ISR_NO_ERR 107
ISR_NO_ERR 108
ISR_NO_ERR 109
ISR_NO_ERR 110
ISR_NO_ERR 111
ISR_NO_ERR 112
ISR_NO_ERR 113
ISR_NO_ERR 114
ISR_NO_ERR 115
ISR_NO_ERR 116
ISR_NO_ERR 117
ISR_NO_ERR 118
ISR_NO_ERR 119
ISR_NO_ERR 120
ISR_NO_ERR 121
ISR_NO_ERR 122
ISR_NO_ERR 123
ISR_NO_ERR 124
ISR_NO_ERR 125
ISR_NO_ERR 126
ISR_NO_ERR 127
ISR_NO_ERR 128
ISR_NO_ERR 129
ISR_NO_ERR 130
ISR_NO_ERR 131
ISR_NO_ERR 132
ISR_NO_ERR 133
ISR_NO_ERR 134
ISR_NO_ERR 135
ISR_NO_ERR 136
ISR_NO_ERR 137
ISR_NO_ERR 138
ISR_NO_ERR 139
ISR_NO_ERR 140
ISR_NO_ERR 141
ISR_NO_ERR 142
ISR_NO_ERR 143
ISR_NO_ERR 144
ISR_NO_ERR 145
ISR_NO_ERR 146
ISR_NO_ERR 147
ISR_NO_ERR 148
ISR_NO_ERR 149
ISR_NO_ERR 150
ISR_NO_ERR 151
ISR_NO_ERR 152
ISR_NO_ERR 153
ISR_NO_ERR 154
ISR_NO_ERR 155
ISR_NO_ERR 156
ISR_NO_ERR 157
ISR_NO_ERR 158
ISR_NO_ERR 159
ISR_NO_ERR 160
ISR_NO_ERR 161
ISR_NO_ERR 162
ISR_NO_ERR 163
ISR_NO_ERR 164
ISR_NO_ERR 165
ISR_NO_ERR 166
ISR_NO_ERR 167
ISR_NO_ERR 168
ISR_NO_ERR 169
ISR_NO_ERR 170
ISR_NO_ERR 171
ISR_NO_ERR 172
ISR_NO_ERR 173
ISR_NO_ERR 174
ISR_NO_ERR 175
ISR_NO_ERR 176
ISR_NO_ERR 177
ISR_NO_ERR 178
ISR_NO_ERR 179
ISR_NO_ERR 180
ISR_NO_ERR 181
ISR_NO_ERR 182
ISR_NO_ERR 183
ISR_NO_ERR 184
ISR_NO_ERR 185
ISR_NO_ERR 186
ISR_NO_ERR 187
ISR_NO_ERR 188
ISR_NO_ERR 189
ISR_NO_ERR 190
ISR_NO_ERR 191
ISR_NO_ERR 192
ISR_NO_ERR 193
ISR_NO_ERR 194
ISR_NO_ERR 195
ISR_NO_ERR 196
ISR_NO_ERR 197
ISR_NO_ERR 198
ISR_NO_ERR 199
ISR_NO_ERR 200
ISR_NO_ERR 201
ISR_NO_ERR 202
ISR_NO_ERR 203
ISR_NO_ERR 204
ISR_NO_ERR 205
ISR_NO_ERR 206
ISR_NO_ERR 207
ISR_NO_ERR 208
ISR_NO_ERR 209
ISR_NO_ERR 210
ISR_NO_ERR 211
ISR_NO_ERR 212
ISR_NO_ERR 213
ISR_NO_ERR 214
ISR_NO_ERR 215
ISR_NO_ERR 216
ISR_NO_ERR 217
ISR_NO_ERR 218
ISR_NO_ERR 219
ISR_NO_ERR 220
ISR_NO_ERR 221
ISR_NO_ERR 222
ISR_NO_ERR 223
ISR_NO_ERR 224
ISR_NO_ERR 225
ISR_NO_ERR 226
ISR_NO_ERR 227
ISR_NO_ERR 228
ISR_NO_ERR 229
ISR_NO_ERR 230
ISR_NO_ERR 231
ISR_NO_ERR 232
ISR_NO_ERR 233
ISR_NO_ERR 234
ISR_NO_ERR 235
ISR_NO_ERR 236
ISR_NO_ERR 237
ISR_NO_ERR 238
ISR_NO_ERR 239
ISR_NO_ERR 240
ISR_NO_ERR 241
ISR_NO_ERR 242
ISR_NO_ERR 243
ISR_NO_ERR 244
ISR_NO_ERR 245
ISR_NO_ERR 246
ISR_NO_ERR 247
ISR_NO_ERR 248
ISR_NO_ERR 249
ISR_NO_ERR 250
ISR_NO_ERR 251
ISR_NO_ERR 252
ISR_NO_ERR 253
ISR_NO_ERR 254
ISR_NO_ERR 255
