PREFIX=i386-elf

CC=$(PREFIX)-gcc
LD=$(PREFIX)-ld
AS=$(PREFIX)-as
EMU=qemu-system-i386

BIN=bin
OS=$(BIN)/os.img

ROOT=root

STAGE1BIN=$(BIN)/stage1.bin
STAGE1SRC=$(wildcard src/boot/stage1/*.S)

STAGE2BIN=$(BIN)/stage2.bin
STAGE2CSRC=$(wildcard src/boot/stage2/*.c) $(wildcard src/boot/stage2/disk/*.c)
STAGE2ASMSRC=$(wildcard src/boot/stage2/*.S)
STAGE2TARGETS=$(STAGE2ASMSRC:.S=.o) $(STAGE2CSRC:.c=.o)
STAGE2TARGETS:=$(subst src/boot,$(BIN),$(STAGE2TARGETS))

# physical memory address to load the kernel
KERNELLOAD=0x100000
# name of the kernel file
KERNELNAME=kernel.elf
KERNEL=$(BIN)/$(KERNELNAME)
KERNELCSRC=$(wildcard src/kernel/*.c)
KERNELASMSRC=$(wildcard src/kernel/*.S)
KERNELTARGETS=$(KERNELCSRC:.c=.o) $(KERNELASMSRC:.S=.o)
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
CFLAGS:=$(CFLAGS) -DKERNEL_LOAD=$(KERNELLOAD) -DKERNEL_NAME='"/$(KERNELNAME)"'
CFLAGS:=$(CFLAGS) -Isrc/ -fno-pic -static -fno-strict-aliasing -MD -no-pie
CFLAGS:=$(CFLAGS) -fno-omit-frame-pointer -Wunused -Os
LDFLAGS=-nostdlib -static -Isrc/ -m elf_i386
ASFLAGS=--32
KERNELFLAGS=-c -Wall -Wextra -Wpedantic -ffreestanding -nostdlib -Wno-pointer-arith
KERNELFLAGS:=$(KERNELFLAGS) -fno-pie -fno-stack-protector -fno-builtin -fno-builtin-function
KERNELFLAGS:=$(KERNELFLAGS) -Isrc/ -fno-pic -DKERNEL_LOAD=$(KERNELLOAD) -Wunused -O2

BOOTFLAGS=-defsym S2LOC=$(STAGE2_LOCATION) -defsym S2SEG=$(STAGE2_SEGMENT) -defsym S2OFF=$(STAGE2_OFFSET) -defsym S2SIZ=$(STAGE2_SIZE)

all: OS
	
$(BIN):
	mkdir -p $(BIN)
	mkdir -p $(BIN)/stage2
	mkdir -p $(BIN)/kernel

$(STAGE1BIN): $(BIN) $(STAGE1SRC)
	$(AS) -o $(STAGE1BIN).o $(STAGE1SRC) $(ASFLAGS) $(BOOTFLAGS)
	$(LD) -o $(STAGE1BIN) $(STAGE1BIN).o -Ttext 0x7C00 --oformat binary -e _start

$(BIN)/stage2/%.o: src/boot/stage2/%.S
	$(AS) -o $@ $^ $(ASFLAGS)

$(BIN)/stage2/%.o: src/boot/stage2/%.c
	$(CC) -o $@ $^ $(CFLAGS)

$(STAGE2BIN): $(BIN) $(STAGE2TARGETS) src/boot/stage2/link.ld
	$(LD) -o $(STAGE2BIN) $(STAGE2TARGETS) $(LDFLAGS) -Tsrc/boot/stage2/link.ld

$(BIN)/kernel/%.o: src/kernel/%.c
	$(CC) -o $@ $^ $(KERNELFLAGS)

$(BIN)/kernel/%.o: src/kernel/%.S
	$(CC) -o $@ $^ $(KERNELFLAGS)
	# nasm -o $@ $^ -felf32

$(KERNEL): $(BIN) $(KERNELTARGETS) src/kernel/link.ld
	$(LD) -o $(KERNEL) $(KERNELTARGETS) -Tsrc/kernel/link.ld --gc-sections
	
$(ROOT):
	mkdir -p $(ROOT)

run: OS Makefile
	$(EMU) -drive format=raw,file=$(OS) -m 64 -monitor stdio -full-screen -action reboot=shutdown -action shutdown=pause -D trace.log -d int

clean:
	rm -rf $(BIN) $(ROOT)

OS: ./mkfat $(ROOT) $(STAGE1BIN) $(STAGE2BIN) $(KERNEL) Makefile
	cp $(KERNEL) $(ROOT)/$(KERNELNAME)
	./mkfat 32 $(OS) -S$(ROOT) -B$(STAGE1BIN) -R$(STAGE2BIN) -VLEGENDARY

