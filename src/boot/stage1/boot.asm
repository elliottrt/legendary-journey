[bits 16]
[org 0x7C00]

; See http://ntfs.com/fat-boot-modif.htm

fsjump:

	jmp short start
	nop

fsheaders:

; general fat header

oem_name:               db "01234567"       ; must be 8 bytes
bytes_per_sector:       dw 512
sectors_per_cluster:    db 1
reserved_sectors:       dw 1
fat_count:              db 2
root_entries:           dw 0x00E0
total_sectors:          dw 2880
media_descriptor:       db 0xF0
sectors_per_fat:        dw 0                ; fat12/16 only
sectors_per_track:      dw 18
heads:                  dw 2
hidden_sectors:         dd 0
large_sector_count:     dd 0

; fat32 specific header

sectors_per_fat32:      dd 0
flags32:                dw 0
fat_version32:          dw 0
root_dir_cluster32:     dd 2
fsinfo_sector32:        dw 0
backup_boot_sector:     dw 0
reserved32:             dw 0, 0, 0, 0, 0, 0

drive_num:              db 0
reserved0:				db 0
signature:				db 0x29 			; 0x28 or 0x29
serial:					dd 0
volume_label:			db "0123456789A"	; must be 11 bytes
system_identifier:		db "FAT32   "		; must be 8 bytes

; boot code

start:

	mov [drive_num], dl

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

	jnc load_stage2
	cmp bx, 0x55AA
	je load_stage2

ext_err:
	mov ah, 0x0E
	mov al, 'E'
	int 0x10
	mov al, 'X'
	int 0x10
	mov al, 'T'
	int 0x10
	jmp $

load_stage2:

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

	call disk_read

	xor ch, ch
	shl cx, 5
	mov di, es
	add di, cx
	mov es, di

	jmp load_stage2.loop

.finish:

	mov dl, [drive_num]
	; mov si, [boot_segment]
	; mov di, [boot_offset]

	mov ax, S2SEG         ; set segment registers
    mov ds, ax
    mov es, ax

	jmp S2SEG:S2OFF

	mov ah, 0x0E
	mov al, 'D'
	int 0x10
	mov al, 'O'
	int 0x10
	mov al, 'N'
	int 0x10
	mov al, 'E'
	int 0x10

	jmp $

disk_read:
	push ax
	push dx
	push si

	mov dword [disk_read_packet.lba], S2LOC
	mov word [disk_read_packet.segment], S2SEG
	mov word [disk_read_packet.offset], S2OFF
	mov word [disk_read_packet.blocks], S2SIZ

	mov ah, 0x42
	mov di, 3 								; retry count
	mov si, disk_read_packet

.retry:
	stc
	int 0x13
	jnc .done

	; disk error, so reset and try again

	push ax
	mov ah, 0
	stc
	int 0x13
	jc ext_err
	pop ax
	
	dec di
	test di, di
	jnz .retry

.done:

	pop si
	pop dx
	pop ax
	ret

disk_read_packet:
.size:	 	db 0x10
.resr:		db 0x00
.blocks:	dw 0x0000
.offset:	dw 0x0000
.segment:	dw 0x0000
.lba:		dq 0

times 510 - ($-$$) db 0
dw 0xAA55

