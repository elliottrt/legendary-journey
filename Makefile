CC=i386-elf-gcc
LD=i386-elf-ld
AS=nasm
EMU=qemu-system-i386

CFLAGS=-m32 -ansi -c -Wall -Wextra -Wpedantic -ffreestanding -nostdlib -fno-pie -fno-stack-protector -fno-builtin -fno-builtin-function
LDFLAGS=-nostdlib -static
ASFLAGS=

BIN=bin
OUT=$(BIN)/os.bin

STAGE1BIN=$(BIN)/stage1.bin
STAGE1SRC=$(wildcard src/boot/stage1/*.asm)

STAGE2BIN=$(BIN)/stage2.bin
STAGE2CSRC=$(wildcard src/boot/stage2/*.c)
STAGE2ASMSRC=$(wildcard src/boot/stage2/*.asm)
STAGE2TARGETS=$(STAGE2ASMSRC:.asm=.o) $(STAGE2CSRC:.c=.o)
STAGE2TARGETS:=$(subst src/boot/stage2,$(BIN),$(STAGE2TARGETS))

# LBA disk location
STAGE2_LOCATION=1
# destination segment
STAGE2_SEGMENT=0
# destination offset
STAGE2_OFFSET=1280
# size in sectors (512 bytes)
STAGE2_SIZE=2

all: run
	

dirs:
	mkdir -p bin

$(STAGE1BIN): dirs $(STAGE1SRC)
	$(AS) -o $(STAGE1BIN) $(STAGE1SRC) $(ASFLAGS) -fbin -DS2LOC=$(STAGE2_LOCATION) \
		  -DS2SEG=$(STAGE2_SEGMENT) -DS2OFF=$(STAGE2_OFFSET) -DS2SIZ=$(STAGE2_SIZE) 

$(BIN)/%.o: src/boot/stage2/%.asm
	$(AS) -o $@ $^ $(ASFLAGS) -felf32

$(BIN)/%.o: src/boot/stage2/%.c
	$(CC) -o $@ $^ $(CFLAGS)

$(STAGE2BIN): dirs $(STAGE2TARGETS)
	$(LD) -o $(STAGE2BIN) $(STAGE2TARGETS) $(LDFLAGS) -Tsrc/boot/stage2/link.ld

run1: $(STAGE1BIN)
	$(EMU) -drive format=raw,file=$(STAGE1BIN) -monitor stdio

gather: $(STAGE1BIN) $(STAGE2BIN)
	dd if=/dev/zero of=$(OUT) bs=512 count=2880
	dd if=$(STAGE1BIN) of=$(OUT) conv=notrunc bs=512 seek=0 count=1
	dd if=$(STAGE2BIN) of=$(OUT) conv=notrunc bs=512 seek=$(STAGE2_LOCATION) count=$(STAGE2_SIZE)

run: gather
	$(EMU) -drive format=raw,file=$(OUT) -monitor stdio


