# legendary-journey
My senior year operating system, written in assembly & ansi C.

I am not at all qualified to do this. The [posted warnings](https://wiki.osdev.org/Beginner_Mistakes#A_Hard_Truth) have been ignored; I have absolutely none of the recommended experience. This is more of an experiment to see how far I can get.

## requirements
- i386-elf-gcc
- i386-elf-ld
- nasm
- qemu-system-i386
- make

## todo/ideas
in no particular order:
- change STAGE2_x in Makefile to be dynamically calculated instead of manually
- disk operations in stage2
- fat32 support in stage2
- figure out how to create a fat32 disk on a file
- find out how much ram I have
- write some standard C library implementations
- get memory map
- load actual GDT
- memory allocation?
- proper stack
- IDT/PIC/PIT
- find CPU features
- APIC
- userspace
- load kernel into memory and jump to it
- maybe add some more to stage1 since I have over 200 bytes left
- actually learn what I'm doing

(list partly from [here](https://wiki.osdev.org/Rolling_Your_Own_Bootloader))