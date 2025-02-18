# legendary-journey
My senior year operating system, written in assembly & C.

I am not at all qualified to do this. The [posted warnings](https://wiki.osdev.org/Beginner_Mistakes#A_Hard_Truth) have been ignored; I have absolutely none of the recommended experience. This is more of an experiment to see how far I can get.

## requirements
- i386-elf-gcc
- i386-elf-ld
- i386-elf-as
- qemu-system-i386
- make
- mkfat (elliottrt/mkfat)

## user space model
- there is only a single running process
- when a system call is issued, control jumps to the operating system then back to the program
- the kernel will act as the shell
- user programs must be compiled with the flags `-nostdlib -Wl,-emain,--warn-unresolved-symbols,-q -Wno-builtin-declaration-mismatch`
	- this will allow undefined functions and variables. if these are not system functions, the OS will warn you and not run the desired program.

## todo/ideas (in no particular order)
- change STAGE2_x in Makefile to be dynamically calculated instead of manually
- allocation for large structures like entries (idt.c) and keyboard (kbd.c) and static files (file.c)
- don't have standard library function implementations duplicated in stage2 and kernel - maybe a #define for each function?
- irq handlers should be as short as possible, try to make kbd::kbdhandler shorter - buffer kbd input
- find CPU features
- video mode
- figure out what video modes are available
- actually learn what I'm doing
- printf %_ should clear the screen? maybe %e for error and %s for normal colors?
- in(b|w)/out(b|w) might need the whole ebp/esp thing

(list partly from [here](https://wiki.osdev.org/Rolling_Your_Own_Bootloader))