# legendary-journey
My senior year operating system, written in assembly & C.

I am not at all qualified to do this. The [posted warnings](https://wiki.osdev.org/Beginner_Mistakes#A_Hard_Truth) have been ignored; I have absolutely none of the recommended experience. This is more of an experiment to see how far I can get.

## requirements
- i386-elf-gcc
- i386-elf-ld
- nasm
- qemu-system-i386
- make
- mkfat (elliottrt/mkfat)

## todo/ideas
in no particular order:
- change STAGE2_x in Makefile to be dynamically calculated instead of manually
- null & safety checks in std.h
- optimize file.c::fileread, we always don't need to read into buffer then memory, we can go straight to memory sometimes
- do what it suggests in notes [here](https://wiki.osdev.org/Detecting_Memory_(x86)#BIOS_Function:_INT_0x15.2C_EAX_.3D_0xE820)
- load actual GDT
- multitasking/threading
- memory allocation?
- IDT/PIC/PIT
- find CPU features
- APIC
- userspace
- video mode
- figure out what video modes are available
- maybe add some more to stage1 since I have over 200 bytes left
- actually learn what I'm doing

(list partly from [here](https://wiki.osdev.org/Rolling_Your_Own_Bootloader))