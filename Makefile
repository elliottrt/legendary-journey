CC=i386-elf-gcc
LD=i386-elf-ld
AS=nasm
EMU=qemu-system-i386

BIN=bin
OS=$(BIN)/os.img

STAGE1BIN=$(BIN)/stage1.bin
STAGE1SRC=$(wildcard src/boot/stage1/*.asm)

STAGE2BIN=$(BIN)/stage2.bin
STAGE2CSRC=$(wildcard src/boot/stage2/*.c) $(wildcard src/boot/stage2/disk/*.c)
STAGE2ASMSRC=$(wildcard src/boot/stage2/*.asm)
STAGE2TARGETS=$(STAGE2ASMSRC:.asm=.o) $(STAGE2CSRC:.c=.o)
STAGE2TARGETS:=$(subst src/boot,$(BIN),$(STAGE2TARGETS))

# physical memory address to load the kernel
KERNELLOAD=0x100000
# name of the kernel file
KERNELNAME=kernel.elf
KERNEL=$(BIN)/$(KERNELNAME)
KERNELCSRC=$(wildcard src/kernel/*.c)
KERNELASMSRC=$(wildcard src/kernel/*.asm)
KERNELTARGETS=$(KERNELCSRC:.c=.o) $(KERNELASMSRC:.asm=.o)
KERNELTARGETS:=$(subst src,$(BIN),$(KERNELTARGETS))

# LBA disk location
STAGE2_LOCATION=8
# destination segment
STAGE2_SEGMENT=0
# destination offset
STAGE2_OFFSET=0x7E00
# size in sectors (512 bytes)
STAGE2_SIZE=24

# See https://www.rapidtables.com/code/linux/gcc/gcc-o.html#optimization
CFLAGS=-m32 -c -Wall -Wextra -Wpedantic -ffreestanding -nostdlib -Wno-pointer-arith
CFLAGS:=$(CFLAGS) -fno-pie -fno-stack-protector -fno-builtin -fno-builtin-function
CFLAGS:=$(CFLAGS) -Os -DKERNEL_LOAD=$(KERNELLOAD) -DKERNEL_NAME='"/$(KERNELNAME)"'
CFLAGS:=$(CFLAGS) -Isrc/
LDFLAGS=-nostdlib -static
ASFLAGS=
KERNELFLAGS=-m32 -c -Wall -Wextra -Wpedantic -ffreestanding -nostdlib -Wno-pointer-arith
KERNELFLAGS:=$(KERNELFLAGS) -fno-pie -fno-stack-protector -fno-builtin -fno-builtin-function
KERNELFLAGS:=$(KERNELFLAGS) -Isrc/

all: run
	
$(BIN): Makefile
	mkdir -p $(BIN)
	mkdir -p $(BIN)/stage2
	mkdir -p $(BIN)/kernel

$(STAGE1BIN): $(BIN) $(STAGE1SRC) Makefile
	$(AS) -o $(STAGE1BIN) $(STAGE1SRC) $(ASFLAGS) -fbin -DS2LOC=$(STAGE2_LOCATION) -DS2SEG=$(STAGE2_SEGMENT) -DS2OFF=$(STAGE2_OFFSET) -DS2SIZ=$(STAGE2_SIZE) 

$(BIN)/stage2/%.o: src/boot/stage2/%.asm
	$(AS) -o $@ $^ $(ASFLAGS) -felf32

$(BIN)/stage2/%.o: src/boot/stage2/%.c
	$(CC) -o $@ $^ $(CFLAGS)

$(STAGE2BIN): $(BIN) $(STAGE2TARGETS) src/boot/stage2/link.ld Makefile
	$(LD) -o $(STAGE2BIN) $(STAGE2TARGETS) $(LDFLAGS) -Tsrc/boot/stage2/link.ld

$(BIN)/kernel/%.o: src/kernel/%.c
	$(CC) -o $@ $^ $(KERNELFLAGS)

$(BIN)/kernel/%.o: src/kernel/%.asm
	$(AS) -o $@ $^ $(ASFLAGS) -felf32

$(KERNEL): $(BIN) $(KERNELTARGETS) src/kernel/link.ld Makefile
	$(LD) -o $(KERNEL) $(KERNELTARGETS) $(KERNEL_FLAGS) -Tsrc/kernel/link.ld
	cp $(KERNEL) root/$(KERNELNAME)

run: $(OS) Makefile
	$(EMU) -drive format=raw,file=$(OS) -m 128M -monitor stdio -full-screen

clean:
	rm -rf $(BIN)

$(OS): ./mkfat $(STAGE1BIN) $(STAGE2BIN) $(KERNEL) Makefile
	./mkfat 32 $(OS) -Sroot -B$(STAGE1BIN) -R$(STAGE2BIN) -VLEGENDARY

