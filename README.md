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

## todo/ideas
in no particular order:
- change STAGE2_x in Makefile to be dynamically calculated instead of manually
- allocation for large structures like entries (idt.c) and keyboard (kbd.c)
- don't have standard library function implementations duplicated in stage2 and kernel - maybe a #define for each function?
- irq handlers should be as short as possible, try to make kbd::kbdhandler shorter - AND DON'T call PRINTF
- and buffer kbd input
- optimize stage2::file.c::fileread, we always don't need to read into buffer then memory, we can go straight to memory sometimes
- do what it suggests in notes [here](https://wiki.osdev.org/Detecting_Memory_(x86)#BIOS_Function:_INT_0x15.2C_EAX_.3D_0xE820)
- memory allocation?
- find CPU features
- userspace
- video mode
- figure out what video modes are available
- actually learn what I'm doing
- printf %_ should clear the screen? maybe %e for error and %s for normal colors?
- in(b|w)/out(b|w) might need the whole ebp/esp thing
- kernel::file.c::fileopen should create a new file if one doesn't exist - flags? (O_CREAT)
- fat.c should have function to update all FATs

(list partly from [here](https://wiki.osdev.org/Rolling_Your_Own_Bootloader))