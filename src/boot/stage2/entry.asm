bits 16
extern stage2main
global _entry

section .entry

_entry:

	mov [boot_drive], dl

	cli

	; mov [boot_segment], si
    ; mov [boot_offset], di

	mov ax, ds
    mov ss, ax
    mov sp, 0xFFF0
    mov bp, sp

	call memdetect

	; enable A20
	call a20_enable

	lgdt [GDT_desc]

	mov eax, cr0
    or al, 1
    mov cr0, eax

	; hlt

	jmp dword CODE_SEG:protected

[bits 32]
protected:

	mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax

	; mov dx, [boot_segment]
    ; shl edx, 16
    ; mov dx, [boot_offset]
    ; push edx

    ; xor edx, edx
    ; mov dl, [boot_drive]
    ; push edx
	
    call stage2main

	hlt

[bits 16]
; https://wiki.osdev.org/A20#Testing_the_A20_line
a20_enable:

	call a20_wait_in
	mov al, 0xAD
	out 0x64, al

	call a20_wait_in
	mov al, 0xD0
    out 0x64, al

	call a20_wait_out
    in al, 0x60
    push eax

	call a20_wait_in
    mov al, 0xD1
    out 0x64, al

	call a20_wait_in
    pop eax
    or al, 2
    out 0x60, al

	call a20_wait_in
    mov al, 0xAE
    out 0x64, al

    call a20_wait_in
	ret

a20_wait_in:
    in al, 0x64
    test al, 2
    jnz a20_wait_in
    ret

a20_wait_out:
    in al, 0x64
    test al, 1
    jz a20_wait_out
    ret

; See https://wiki.osdev.org/Detecting_Memory_(x86)#Getting_an_E820_Memory_Map
memdetect:
	push eax
	push ebx
	push ecx
	push edx
	push ebp
	push edi

	mov di, _entry_memregions
	xor ebx, ebx
	xor bp, bp
	mov edx, 0x534D4150
	mov eax, 0xE820
	mov [es:di + 20], dword 1
	mov ecx, 24
	int 0x15
	jc .failed
	mov edx, 0x0534D4150
	cmp eax, edx
	jne .failed
	test ebx, ebx
	je .failed
	jmp .jmpin
.setup:
	mov eax, 0xE820
	mov [es:di + 20], dword 1
	mov ecx, 24
	int 0x15
	jc .finish
	mov edx, 0x534D4150
.jmpin:
	jcxz .skipent
	cmp cl, 20
	jbe .notext
	test byte [es:di + 20], 1
	je .skipent
.notext:
	mov ecx, [es:di + 8]
	or ecx, [es:di + 12]
	jz .skipent
	inc bp
	add di, 24
.skipent:
	test ebx, ebx
	jne .setup
.finish
	mov [_entry_memcount], bp
	clc
	pop edi
	pop ebp
	pop edx
	pop ecx
	pop ebx
	pop eax
	ret
.failed:
	stc
	pop edi
	pop ebp
	pop edx
	pop ecx
	pop ebx
	pop eax
	ret

section .data

CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start

boot_drive: db 0x00

; See https://wiki.osdev.org/GDT
GDT_start:
GDT_null:
	dq 0

GDT_code:
	dw 0xFFFF 		; limit
	dw 0x0000		; base lo
	db 0x00			; base mid
	db 10011010b	; access flags
	db 11001111b	; granularity
	db 0x00			; base hi

GDT_data:
	dw 0xFFFF 		; limit
	dw 0x0000		; base lo
	db 0x00			; base mid
	db 10010010b	; access flags
	db 11001111b	; granularity
	db 0x00			; base hi

GDT_end:

GDT_desc:
	dw GDT_desc - GDT_start - 1
	dd GDT_start

global _entry_memcount
_entry_memcount:
	dw 0 ; region count
global _entry_memregions
_entry_memregions:
	times (8 * 24) db 0; region data (8 region spots)
