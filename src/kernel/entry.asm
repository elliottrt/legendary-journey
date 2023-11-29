bits 32

extern main

section .text

global _start
_start:

	; get first argument
	mov eax, [esp+4]
	mov [disksectors], eax

	; make an actual stack
	mov esp, stackbase

	call main

section .bss

stack:
	resb 4096
stackbase:

global disksectors
disksectors:
	resd 1
