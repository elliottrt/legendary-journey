#include "mmu.h"

.code32

.extern main
.extern entrypgdir

.section .text

// booting will jump to phys start, and we want to offset this
// so virtual memory is ready to be set up
.globl _start
_start = V2P_WO(_start_virt)

_start_virt:

	// all paging stuff needs to be before variables passed by boot/stage2
	mov %cr4, %eax
    orl $0x00000010, %eax  // 4096 byte pages
    mov %eax, %cr4
	
	movl $(V2P_WO(entrypgdir)), %eax
    movl %eax, %cr3

    movl %cr0, %eax
    orl $0x80010000, %eax // paging, write protection
    movl %eax, %cr0

	// get first argument
	movl 4(%esp), %eax
	movl %eax, (disksectors)

	// get second argument
	movl 8(%esp), %eax
	movl %eax, (PHYSTOP)

	// make an actual stack
	movl $(stack + 0x1000), %esp

	// i spent two weeks trying to fix a page fault
	// and all i needed was this instead of 'call main'
	mov $main, %eax
  	jmp *%eax

.comm stack, 0x1000

.section .bss

.globl disksectors
disksectors:
	.space 4
.globl PHYSTOP
PHYSTOP:
	.space 4
