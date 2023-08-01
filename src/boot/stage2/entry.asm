bits 16
extern stage2main
global _entry

section .entry

_entry:

	cli

	mov [boot_drive], dl
	; mov [boot_segment], si
    ; mov [boot_offset], di

	mov ax, ds
    mov ss, ax
    mov sp, 0xFFF0
    mov bp, sp

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
    or al, 2                                    ; bit 2 = A20 bit
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

; 16 bit code
	dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF
	dw 0                        ; base (bits 0-15) = 0x0
	db 0                        ; base (bits 16-23)
	db 10011010b                ; access (present, ring 0, code segment, executable, direction 0, readable)
	db 00001111b                ; granularity (1b pages, 16-bit pmode) + limit (bits 16-19)
	db 0

; 16 bit data
	dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF
	dw 0                        ; base (bits 0-15) = 0x0
	db 0                        ; base (bits 16-23)
	db 10010010b                ; access (present, ring 0, data segment, executable, direction 0, writable)
	db 00001111b                ; granularity (1b pages, 16-bit pmode) + limit (bits 16-19)
	db 0                        ; base high

GDT_end:

GDT_desc:
	dw GDT_desc - GDT_start - 1
	dd GDT_start
