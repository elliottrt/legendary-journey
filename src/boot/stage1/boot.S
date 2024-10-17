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
bytespersector:	       	.2byte 512
sectorspercluster:	   	.byte 1
reservedsectors:       	.2byte 32
fatcount:              	.byte 2
rootentries:           	.2byte 0 				// fat12/16 only
totalsectors:          	.2byte 0 				// we dont know right now
mediadescriptor:      	.byte 0xF0
sectorsperfat:        	.2byte 0                	// fat12/16 only
sectorspertrack: 	    .2byte 0
heads:                  .2byte 0
hiddensectors:         	.4byte 0
largesectorcount:    	.4byte 0

// fat32 specific header

sectorsperfat32:      	.4byte 0
flags32:                .2byte 0
fatversion32:          	.2byte 0
rootdircluster:     	.4byte 2
fsinfosector:        	.2byte 0
backupbootsector:     	.2byte 0
reserved32:             .2byte 0, 0, 0, 0, 0, 0

drivenum:              	.byte 0
reserved0:				.byte 0
signature:				.byte 0x29 			// 0x28 or 0x29
serial:					.4byte 0
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
	movb $'B', %al
	int $0x10
	movb $'O', %al
	int $0x10
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
.blocks:	.2byte S2SIZ
.offset:	.2byte S2OFF
.segment:	.2byte S2SEG
.lba:		.4byte S2LOC

.fill 510 - (. - _start)
.2byte 0xAA55

