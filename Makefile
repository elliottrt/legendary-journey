PREFIX=i386-elf

CC=$(PREFIX)-gcc
LD=$(PREFIX)-ld
AS=$(PREFIX)-as
EMU=qemu-system-i386

OS=os.img
ROOT=root

SOURCE_DIR=src
STAGE1_DIR=$(SOURCE_DIR)/boot/stage1
STAGE2_DIR=$(SOURCE_DIR)/boot/stage2
KERNEL_DIR=$(SOURCE_DIR)/kernel

STAGE1BIN=$(STAGE1_DIR)/stage1.bin
STAGE1SRC=$(wildcard $(STAGE1_DIR)/*.S)

STAGE2BIN=$(STAGE2_DIR)/stage2.bin
STAGE2CSRC=$(wildcard $(STAGE2_DIR)/*.c) $(wildcard $(STAGE2_DIR)/disk/*.c)
STAGE2ASMSRC=$(wildcard $(STAGE2_DIR)/*.S)
STAGE2TARGETS=$(STAGE2ASMSRC:.S=.o) $(STAGE2CSRC:.c=.o)

# name of the kernel file
KERNELNAME=kernel
KERNEL=$(ROOT)/$(KERNELNAME)
KERNELCSRC=$(wildcard $(KERNEL_DIR)/*.c) $(wildcard $(KERNEL_DIR)/*/*.c)
KERNELASMSRC=$(wildcard $(KERNEL_DIR)/*.S) $(wildcard $(KERNEL_DIR)/*/*.S)
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
# place to load user code at (seems to be a linux default)
USERBASE=0x08048000

# width of tabs in terminal
TAB_WIDTH=4

# See https://www.rapidtables.com/code/linux/gcc/gcc-o.html#optimization
CFLAGS=-m32 -c -Wall -Wextra -Wpedantic -ffreestanding -nostdlib -Wno-pointer-arith
CFLAGS:=$(CFLAGS) -fno-pie -fno-stack-protector -fno-builtin -fno-builtin-function
CFLAGS:=$(CFLAGS) -DKERNEL_NAME='"/$(KERNELNAME)"' -Isrc/
CFLAGS:=$(CFLAGS) -fno-pic -static -fno-strict-aliasing -no-pie
CFLAGS:=$(CFLAGS) -fno-omit-frame-pointer -Wunused -O2
LDFLAGS=-nostdlib -static -m elf_i386
ASFLAGS=--32
KERNELFLAGS=-c -Wall -Wextra -Wpedantic -ffreestanding -nostdlib -Wno-pointer-arith
KERNELFLAGS:=$(KERNELFLAGS) -fno-pie -fno-stack-protector -fno-builtin -fno-builtin-function
KERNELFLAGS:=$(KERNELFLAGS) -fno-pic -Wunused -O2 -DTAB_WIDTH=$(TAB_WIDTH) -DUSERBASE=$(USERBASE)
KERNELFLAGS:=$(KERNELFLAGS) -DKERNBASE=$(KERNBASE) -I$(KERNEL_DIR) -Isrc/

BOOTFLAGS=-defsym S2LOC=8 -defsym S2OFF=$(STAGE2_OFFSET) -defsym S2SIZ=24

.PHONY: all run clean user _user

all: $(OS)

user:
	$(CC) -nostdlib -o user/ret -Wl,-emain user/ret.c
	cp user/ret root/ret
	objdump -h -x -z -d user/ret
	touch $(ROOT)

$(STAGE1BIN): $(BIN) $(STAGE1SRC)
	$(AS) -o $(STAGE1BIN).o $(STAGE1SRC) $(ASFLAGS) $(BOOTFLAGS)
	$(LD) -o $(STAGE1BIN) $(STAGE1BIN).o -Ttext 0x7C00 --oformat binary -e _start

$(STAGE2_DIR)/%.o: $(STAGE2_DIR)/%.S
	$(AS) -o $@ $^ $(ASFLAGS)

$(STAGE2_DIR)/%.o: $(STAGE2_DIR)/%.c
	$(CC) -o $@ $^ $(CFLAGS)

$(STAGE2BIN): $(BIN) $(STAGE2TARGETS) $(STAGE2_DIR)/link.ld
	$(LD) -o $(STAGE2BIN) $(STAGE2TARGETS) $(LDFLAGS) -T$(STAGE2_DIR)/link.ld

$(KERNEL_DIR)/%.o: $(KERNEL_DIR)/%.c
	$(CC) -o $@ $^ $(KERNELFLAGS)

$(KERNEL_DIR)/%.o: $(KERNEL_DIR)/%.S
	$(CC) -o $@ $^ $(KERNELFLAGS)

$(KERNEL): $(BIN) $(KERNELTARGETS) $(KERNEL_DIR)/link.ld
	$(LD) -o $(KERNEL) $(KERNELTARGETS) -T$(KERNEL_DIR)/link.ld

run: $(OS)
	$(EMU) -drive format=raw,file=$(OS) -m 128 -monitor stdio -action reboot=shutdown -action shutdown=pause -D trace.log -d int

clean:
	$(RM) -r $(KERNEL)
	$(RM) $(KERNELTARGETS)
	$(RM) $(STAGE2TARGETS)

$(ROOT):
	mkdir -p $(ROOT)

$(OS): $(ROOT) $(STAGE1BIN) $(STAGE2BIN) $(KERNEL)
	./mkfat 32 $(OS) -S$(ROOT) -B$(STAGE1BIN) -R$(STAGE2BIN) -VLEGENDARY

