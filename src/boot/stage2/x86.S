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
