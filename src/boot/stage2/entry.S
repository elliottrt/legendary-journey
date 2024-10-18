.code16
.extern stage2main
.globl _entry

.section .text.prologue

_entry:
    
	call memdetect

	cli

	movw %ds, %ax
    movw %ax, %ss
    movw $0xFFF0, %sp 
    movw %sp, %bp

	// enable A20
	call a20enable

	// disable VGA cursor
	call disablecursor

	lgdt (GDT_desc)

	movl %cr0, %eax
    orb $1, %al
    movl %eax, %cr0

	ljmpl $CODE_SEG, $protected

.code32
protected:

	movw $DATA_SEG, %ax
    movw %ax, %ds
    movw %ax, %ss
	movw %ax, %es
	
    call stage2main

    hlt

.code16

// https://wiki.osdev.org/Text_Mode_Cursor
disablecursor:
	pushw %ax
	pushw %dx

	movw $0x3D4, %dx
	movb $0xA, %al 		// low cursor shape register
	outb %al, %dx
 
 	incw %dx
	movb $0x20, %al 	// bits 6-7 unused, bit 5 disables the cursor, bits 0-4 control the cursor shape
	outb %al, %dx

	popw %dx
	popw %ax
	ret

// https://wiki.osdev.org/A20#Testing_the_A20_line
a20enable:

	call a20waitin
	movb $0xAD, %al
	outb %al, $0x64

	call a20waitin
	movb $0xD0, %al
    outb %al, $0x64

	call a20waitout
    inb $0x60, %al
    pushl %eax

	call a20waitin
    movb $0xD1, %al
    outb %al, $0x64

	call a20waitin
    popl %eax
    orb $2, %al
    outb %al, $0x60

	call a20waitin
    movb $0xAE, %al 
    outb %al, $0x64

    call a20waitin
	ret

a20waitin:
    inb $0x64, %al
    testb $2, %al
    jnz a20waitin
    ret

a20waitout:
    inb $0x64, %al
    testb $1, %al
    jz a20waitout
    ret

// See https://wiki.osdev.org/Detecting_Memory_(x86)#Getting_an_E820_Memory_Map
memdetect:
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %ebp
	pushl %edi

	movw $_memregions, %di
	xorl %ebx, %ebx
	xorw %bp, %bp
	movl $0x534D4150, %edx
	movl $0xE820, %eax
	movl $1, %es:20(%di)
	movl $24, %ecx 
	int $0x15
	jc .failed
	movl $0x0534D4150, %edx
	cmpl %edx, %eax
	jne .failed
	testl %ebx, %ebx
	je .failed
	jmp .jmpin
.setup:
	movl $0xE820, %eax
	movl $1, %es:20(%di)
	movl $24, %ecx
	int $0x15
	jc .finish
	movl $0x534D4150, %edx
.jmpin:
	jcxz .skipent
	cmpb $20, %cl 
	jbe .notext
	testb $1, %es:20(%di)
	je .skipent
.notext:
	movl %es:8(%di), %ecx
	orl %es:12(%di), %ecx
	jz .skipent
	incw %bp
	addw $24, %di
.skipent:
	testl %ebx, %ebx
	jne .setup
.finish:
	movw %bp, _memregioncount
	clc
	popl %edi
	popl %ebp
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	ret
.failed:
	stc
	popl %edi
	popl %ebp
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	ret

.section .data

.equ CODE_SEG, GDT_code - GDT_start
.equ DATA_SEG, GDT_data - GDT_start

// See https://wiki.osdev.org/GDT
GDT_start:
GDT_null:
	.8byte 0

GDT_code:
	.2byte 0xFFFF 		// limit
	.2byte 0x0000		// base lo
	.byte 0x00			// base mid
	.byte 0b10011010	// access flags
	.byte 0b11001111	// granularity
	.byte 0x00			// base hi

GDT_data:
	.2byte 0xFFFF 		// limit
	.2byte 0x0000		// base lo
	.byte 0x00			// base mid
	.byte 0b10010010	// access flags
	.byte 0b11001111	// granularity
	.byte 0x00			// base hi

GDT_end:

GDT_desc:
	.2byte GDT_desc - GDT_start - 1
	.4byte GDT_start

.globl _memregioncount
_memregioncount:
	.2byte 0 // region count
.globl _memregions
_memregions:
	.fill (8 * 24) // region data (8 region spots)
