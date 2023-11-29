bits 32

global outb
outb:
	mov dx, [esp + 4]
	mov al, [esp + 8]
    out dx, al
    ret

global inb
inb:
	mov dx, [esp + 4]
    xor eax, eax
    in al, dx
    ret

global outw
outw:
	mov dx, [esp + 4]
	mov ax, [esp + 8]
	out dx, ax
	ret

global inw
inw:
	mov dx, [esp + 4]
	xor eax, eax
	in ax, dx
	ret
