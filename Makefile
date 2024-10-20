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

# name of the kernel file
KERNELNAME=kernel.elf
KERNEL=$(BIN)/$(KERNELNAME)
KERNELCSRC=$(wildcard src/kernel/*.c) $(wildcard src/kernel/*/*.c)
KERNELASMSRC=$(wildcard src/kernel/*.S) $(wildcard src/kernel/*/*.S)
KERNELTARGETS=$(KERNELCSRC:.c=.o) $(KERNELASMSRC:.S=.o)

# LBA disk location
STAGE2_LOCATION=8
# destination segment
STAGE2_SEGMENT=0
# destination offset
STAGE2_OFFSET=0x7E00
# size in sectors (512 bytes)
STAGE2_SIZE=24
# kernel virtual memory location
KERNBASE=0x80000000
# KERNBASE=0xC0000000
# KERNBASE=0xE0000000

# width of tabs in terminal
TAB_WIDTH=4

# See https://www.rapidtables.com/code/linux/gcc/gcc-o.html#optimization
CFLAGS=-m32 -c -Wall -Wextra -Wpedantic -ffreestanding -nostdlib -Wno-pointer-arith
CFLAGS:=$(CFLAGS) -fno-pie -fno-stack-protector -fno-builtin -fno-builtin-function
CFLAGS:=$(CFLAGS) -DKERNEL_NAME='"/$(KERNELNAME)"' -Isrc/
CFLAGS:=$(CFLAGS) -fno-pic -static -fno-strict-aliasing -MD -no-pie
CFLAGS:=$(CFLAGS) -fno-omit-frame-pointer -Wunused -O2
LDFLAGS=-nostdlib -static -m elf_i386
ASFLAGS=--32
KERNELFLAGS=-c -Wall -Wextra -Wpedantic -ffreestanding -nostdlib -Wno-pointer-arith
KERNELFLAGS:=$(KERNELFLAGS) -fno-pie -fno-stack-protector -fno-builtin -fno-builtin-function
KERNELFLAGS:=$(KERNELFLAGS) -fno-pic -Wunused -O2 -DTAB_WIDTH=$(TAB_WIDTH)
KERNELFLAGS:=$(KERNELFLAGS) -DKERNBASE=$(KERNBASE) -Isrc/kernel/ -Isrc/

BOOTFLAGS=-defsym S2LOC=8 -defsym S2OFF=$(STAGE2_OFFSET) -defsym S2SIZ=24

all: $(OS)
	
$(BIN):
	mkdir -p $(BIN)

$(STAGE1BIN): $(BIN) $(STAGE1SRC)
	$(AS) -o $(STAGE1BIN).o $(STAGE1SRC) $(ASFLAGS) $(BOOTFLAGS)
	$(LD) -o $(STAGE1BIN) $(STAGE1BIN).o -Ttext 0x7C00 --oformat binary -e _start

src/boot/stage2/%.o: src/boot/stage2/%.S
	$(AS) -o $@ $^ $(ASFLAGS)

src/boot/stage2/%.o: src/boot/stage2/%.c
	$(CC) -o $@ $^ $(CFLAGS)

$(STAGE2BIN): $(BIN) $(STAGE2TARGETS) src/boot/stage2/link.ld
	$(LD) -o $(STAGE2BIN) $(STAGE2TARGETS) $(LDFLAGS) -Tsrc/boot/stage2/link.ld

src/kernel/%.o: src/kernel/%.c
	$(CC) -o $@ $^ $(KERNELFLAGS)

src/kernel/%.o: src/kernel/%.S
	$(CC) -o $@ $^ $(KERNELFLAGS)
	# nasm -o $@ $^ -felf32

$(KERNEL): $(BIN) $(KERNELTARGETS) src/kernel/link.ld
	$(LD) -o $(KERNEL) $(KERNELTARGETS) -Tsrc/kernel/link.ld

run: $(OS) Makefile
	$(EMU) -drive format=raw,file=$(OS) -m 128 -monitor stdio -action reboot=shutdown -action shutdown=pause -D trace.log -d int

clean:
	rm -rf $(BIN) $(ROOT)
	find . -name '*.o' -delete
	find . -name '*.d' -delete

$(ROOT):
	mkdir -p $(ROOT)

$(OS): ./mkfat $(ROOT) $(STAGE1BIN) $(STAGE2BIN) $(KERNEL)
	cp $(KERNEL) $(ROOT)/$(KERNELNAME)
	./mkfat 32 $(OS) -S$(ROOT) -B$(STAGE1BIN) -R$(STAGE2BIN) -VLEGENDARY

