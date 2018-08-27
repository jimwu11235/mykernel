C_SOURCE = $(shell find ./src/kernel -name "*.c")
C_OBJECT = $(patsubst ./src/kernel/%.c, ./build/%.o, $(C_SOURCE))
S_SOURCE = $(shell find ./src/kernel -name "*.asm")
S_OBJECT = $(patsubst ./src/kernel/%.asm, ./build/%.o, $(S_SOURCE))
BOOT_SOURCE = $(shell find ./src/boot -name "*.asm")
BOOT_BINARY = $(patsubst ./src/boot/%.asm, ./build/%.bin, $(BOOT_SOURCE))
BOOT_IMG = ./build/boot.img

LD_FILE = ./src/kernel/link.ld
LD_OBJ_FILE = ./src/kernel/objlink.ld
KERNEL_BINARY = ./build/kernel.bin

ASM = nasm
CC 	= gcc
LD 	= ld
DD	= dd

C_FLAGS = -m32 -Wall -O -fno-stack-protector -fno-pie -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdlib -nostdinc -fno-builtin -g -I./src/kernel -c
ASM_BOOT_FLAGS = -f bin
ASM_KERNEL_FLAGS = -f elf32
LD_FLAGS = -m elf_i386

all:creat_img run
img:creat_img
deb:creat_img debug

build/%.o:src/kernel/%.c
	$(CC) $(C_FLAGS) $< -o $@

build/%.o:src/kernel/%.asm
	$(ASM) $(ASM_KERNEL_FLAGS) $< -o $@

$(BOOT_BINARY):$(BOOT_SOURCE)
	$(ASM) $(ASM_BOOT_FLAGS) $< -o $@

$(KERNEL_BINARY):$(S_OBJECT) $(C_OBJECT)
	$(LD) $(LD_FLAGS) -T $(LD_FILE) $^ -o $@
	$(LD) $(LD_FLAGS) -oelf32-i386 -T $(LD_OBJ_FILE) $^ -r -o ./build/kernel.o

$(BOOT_IMG):$(BOOT_BINARY)
	$(DD) if=$< of=$@ bs=512 count=2880

creat_img:$(BOOT_IMG) $(KERNEL_BINARY)
	sudo mkdir ./build/fd
	sudo mount -o loop $(BOOT_IMG) ./build/fd
	sleep 1
	sudo cp $(KERNEL_BINARY) ./build/fd
	sudo umount ./build/fd
	sudo rm -r ./build/fd

run:
	qemu -fda $(BOOT_IMG) -boot a

debug:
	qemu -s -S -fda $(BOOT_IMG) -boot a

clear:
	rm $(C_OBJECT) $(S_OBJECT) $(BOOT_BINARY) $(BOOT_IMG) $(KERNEL_BINARY)