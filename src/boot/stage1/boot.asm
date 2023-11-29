[bits 16]
[org 0x7C00]

; See http://ntfs.com/fat-boot-modif.htm

fsjump:

	jmp short start
	nop

fsheaders:

; general fat header

oemname:               	db "01234567"       ; must be 8 bytes
bytespersector:	       	dw 512
sectorspercluster:	   	db 1
reservedsectors:       	dw 32
fatcount:              	db 2
rootentries:           	dw 0 				; fat12/16 only
totalsectors:          	dw 0 				; we don't know right now
mediadescriptor:      	db 0xF0
sectorsperfat:        	dw 0                ; fat12/16 only
sectorspertrack: 	    dw 0
heads:                  dw 0
hiddensectors:         	dd 0
largesectorcount:    	dd 0

; fat32 specific header

sectorsperfat32:      	dd 0
flags32:                dw 0
fatversion32:          	dw 0
rootdircluster:     	dd 2
fsinfosector:        	dw 0
backupbootsector:     	dw 0
reserved32:             dw 0, 0, 0, 0, 0, 0

drivenum:              	db 0
reserved0:				db 0
signature:				db 0x29 			; 0x28 or 0x29
serial:					dd 0
volumelabel:			db "0123456789A"	; must be 11 bytes
systemidentifier:		db "FAT32   "		; must be 8 bytes

; boot code

start:

	mov [drivenum], dl

	; data segment setup
	xor ax, ax
	mov ds, ax
	mov es, ax

	; stack
	mov ss, ax
	mov sp, 0x7C00

	push es
	push word after
	retf

after:

	; check if we have disk extensions
	mov ah, 0x41
	mov bx, 0x55AA
	stc
	int 0x13

	jnc loadstage2
	cmp bx, 0x55AA
	je loadstage2

extensionerror:
	mov ah, 0x0E
	mov al, 'E'
	int 0x10
	mov al, 'X'
	int 0x10
	mov al, 'T'
	int 0x10
	jmp $

loadstage2:

	mov si, S2LOC
	mov ax, S2SEG
	mov es, ax
	mov bx, S2OFF

.loop:

	mov eax, [si]
	add si, 4
	mov cl, [si]
	inc si

	cmp eax, 0
	je .finish

	call diskread

	xor ch, ch
	shl cx, 5
	mov di, es
	add di, cx
	mov es, di

	jmp loadstage2.loop

.finish:

	mov dl, [drivenum]

	mov ax, S2SEG         ; set segment registers
    mov ds, ax
    mov es, ax

	jmp S2SEG:S2OFF

diskread:
	push ax
	push dx
	push si

	mov dword [diskreadpacket.lba], S2LOC
	mov word [diskreadpacket.segment], S2SEG
	mov word [diskreadpacket.offset], S2OFF
	mov word [diskreadpacket.blocks], S2SIZ

	mov ah, 0x42
	mov di, 3 								; retry count
	mov si, diskreadpacket

.retry:
	stc
	int 0x13
	jnc .done

	; disk error, so reset and try again

	push ax
	mov ah, 0
	stc
	int 0x13
	jc extensionerror
	pop ax
	
	dec di
	test di, di
	jnz .retry

.done:

	pop si
	pop dx
	pop ax
	ret

diskreadpacket:
.size:	 	db 0x10
.resr:		db 0x00
.blocks:	dw 0x0000
.offset:	dw 0x0000
.segment:	dw 0x0000
.lba:		dq 0

times 510 - ($-$$) db 0
dw 0xAA55

