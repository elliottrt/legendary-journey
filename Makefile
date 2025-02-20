PREFIX=i386-elf

CC=$(PREFIX)-gcc
LD=$(PREFIX)-ld
AS=$(PREFIX)-as
EMU=qemu-system-i386

OS=os.img
ROOT=root
USER_PROGS=echo ls
USER_PROGS:=$(addprefix $(ROOT)/,$(USER_PROGS))

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
# NOTE: if this is changed, src/kernel/link.ld also needs to be changed
KERNBASE=0x80000000
# place to load user code at (seems to be a linux default)
USERBASE=0x08048000

# width of tabs in terminal
TAB_WIDTH=4

BOOT_ASFLAGS=-defsym S2LOC=8 -defsym S2OFF=$(STAGE2_OFFSET) -defsym S2SIZ=24

# See https://www.rapidtables.com/code/linux/gcc/gcc-o.html#optimization
STAGE2_CFLAGS=-m32 -c -Wall -Wextra -Wpedantic -ffreestanding -nostdlib -Wno-pointer-arith
STAGE2_CFLAGS:=$(STAGE2_CFLAGS) -fno-pie -fno-stack-protector -fno-builtin -fno-builtin-function
STAGE2_CFLAGS:=$(STAGE2_CFLAGS) -DKERNEL_NAME='"/$(KERNELNAME)"' -Isrc/
STAGE2_CFLAGS:=$(STAGE2_CFLAGS) -fno-pic -static -fno-strict-aliasing -no-pie
STAGE2_CFLAGS:=$(STAGE2_CFLAGS) -fno-omit-frame-pointer -Wunused -O2
STAGE2_LDFLAGS=-nostdlib -static -m elf_i386

KERNEL_CFLAGS=-c -Wall -Wextra -Wpedantic -ffreestanding -nostdlib -Wno-pointer-arith
KERNEL_CFLAGS:=$(KERNEL_CFLAGS) -fno-pie -fno-stack-protector -fno-builtin -fno-builtin-function
KERNEL_CFLAGS:=$(KERNEL_CFLAGS) -fno-pic -Wunused -O2 -DTAB_WIDTH=$(TAB_WIDTH) -DUSERBASE=$(USERBASE)
KERNEL_CFLAGS:=$(KERNEL_CFLAGS) -DKERNBASE=$(KERNBASE) -I$(KERNEL_DIR)/ -Isrc/

.PHONY: run clean

$(OS): $(ROOT) $(STAGE1BIN) $(STAGE2BIN) $(KERNEL) $(USER_PROGS)
	./mkfat 32 $(OS) -S$(ROOT) -B$(STAGE1BIN) -R$(STAGE2BIN) -VLEGENDARY

# compilation of user programs
USERFLAGS=-nostdlib -Wl,-emain,--warn-unresolved-symbols,-q
$(ROOT)/%: user/%.c
	$(CC) $(USERFLAGS) -o $@ $^

$(STAGE1BIN): $(BIN) $(STAGE1SRC)
	$(AS) --32 -o $(STAGE1BIN).o $(STAGE1SRC) $(BOOT_ASFLAGS)
	$(LD) -o $(STAGE1BIN) $(STAGE1BIN).o -Ttext 0x7C00 --oformat binary -e _start

$(STAGE2_DIR)/%.o: $(STAGE2_DIR)/%.S
	$(AS) --32 -o $@ $^

$(STAGE2_DIR)/%.o: $(STAGE2_DIR)/%.c
	$(CC) -o $@ $^ $(STAGE2_CFLAGS)

$(STAGE2BIN): $(BIN) $(STAGE2TARGETS) $(STAGE2_DIR)/link.ld
	$(LD) -o $(STAGE2BIN) $(STAGE2TARGETS) $(LDFLAGS) -T$(STAGE2_DIR)/link.ld

$(KERNEL_DIR)/%.o: $(KERNEL_DIR)/%.c
	$(CC) -o $@ $^ $(KERNEL_CFLAGS)

$(KERNEL_DIR)/%.o: $(KERNEL_DIR)/%.S
	$(CC) -o $@ $^ $(KERNEL_CFLAGS)

$(KERNEL): $(BIN) $(KERNELTARGETS) $(KERNEL_DIR)/link.ld
	$(LD) -o $(KERNEL) $(KERNELTARGETS) -T$(KERNEL_DIR)/link.ld

run: $(OS)
	$(EMU) -drive format=raw,file=$(OS) -m 128 -monitor stdio -action reboot=shutdown -action shutdown=pause -D trace.log -d int

clean:
	$(RM) $(OS)
	$(RM) -r $(ROOT)
	$(RM) $(KERNELTARGETS)
	$(RM) $(STAGE2TARGETS)

$(ROOT):
	mkdir -p $(ROOT)

	# test contents for the root directory
	mkdir -p $(ROOT)/folder
	mkdir -p $(ROOT)/folder/dir

	touch $(ROOT)/folder/thing.c
	touch $(ROOT)/folder/test.txt
	touch $(ROOT)/folder/dir/hi
