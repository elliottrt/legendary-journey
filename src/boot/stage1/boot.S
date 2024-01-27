.code16

// See http://ntfs.com/fat-boot-modif.htm

// for linker

.globl _start
_start:
fsjump:

	jmp start
	nop

fsheaders:

// general fat header

oemname:               	.ascii "01234567"       // must be 8 bytes
bytespersector:	       	.word 512
sectorspercluster:	   	.byte 1
reservedsectors:       	.word 32
fatcount:              	.byte 2
rootentries:           	.word 0 				// fat12/16 only
totalsectors:          	.word 0 				// we dont know right now
mediadescriptor:      	.byte 0xF0
sectorsperfat:        	.word 0                	// fat12/16 only
sectorspertrack: 	    .word 0
heads:                  .word 0
hiddensectors:         	.long 0
largesectorcount:    	.long 0

// fat32 specific header

sectorsperfat32:      	.long 0
flags32:                .word 0
fatversion32:          	.word 0
rootdircluster:     	.long 2
fsinfosector:        	.word 0
backupbootsector:     	.word 0
reserved32:             .word 0, 0, 0, 0, 0, 0

drivenum:              	.byte 0
reserved0:				.byte 0
signature:				.byte 0x29 			// 0x28 or 0x29
serial:					.long 0
volumelabel:			.ascii "0123456789A"	// must be 11 bytes
systemidentifier:		.ascii "FAT32   "		// must be 8 bytes

// boot code

start:

	movb %dl, (drivenum)

	// data segment setup
	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %es

	// stack
	movw %ax, %ss
	movw $0x7C00, %sp

	pushw %es
	pushw $after
	retf

after:

	// check if we have disk extensions
	movb $0x41, %ah
	movw $0x55AA, %bx 
	stc
	int $0x13

	jnc loadstage2
	cmpw $0x55AA, %bx
	je loadstage2

extensionerror:
	movb $0x0E, %ah
	movb $'E', %al
	int $0x10
	movb $'X', %al
	int $0x10
	movb $'T', %al
	int $0x10
exterrhlt:
	jmp exterrhlt

loadstage2:

	movw $S2LOC, %si
	movw $S2SEG, %ax 
	movw %ax, %es
	movw $S2OFF, %bx

.loop:

	movl (%si), %eax
	addw $4, %si
	movb (%si), %cl
	inc %si

	cmpl $0, %eax
	je .finish

	call diskread

	xorb %ch, %ch
	shl $5, %cx
	movw %es, %di
	addw %cx, %di
	movw %di, %es

	jmp .loop

.finish:

	movw $S2SEG, %ax          // set segment registers
    movw %ax, %ds
    movw %ax, %es

    hlt
	ljmp $S2SEG, $S2OFF

diskread:
	pushw %ax
	pushw %dx
	pushw %si

	movb $0x42, %ah
	movw $3, %di 								// retry count
	movw $diskreadpacket, %si

.retry:
	stc
	int $0x13
	jnc .done

	// disk error, so reset and try again

	pushw %ax
	movb $0, %ah
	stc
	int $0x13
	jc extensionerror
	popw %ax
	
	dec %di
	test %di, %di
	jnz .retry

.done:

	popw %si
	popw %dx
	popw %ax
	ret

diskreadpacket:
.size:	 	.byte 0x10
.resr:		.byte 0x00
.blocks:	.word S2SIZ
.offset:	.word S2OFF
.segment:	.word S2SEG
.lba:		.long S2LOC

.fill 510 - (. - fsjump)
.word 0xAA55

