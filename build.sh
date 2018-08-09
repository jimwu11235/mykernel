# sudo umount build/fd
# sudo rm -r build/fd
sudo rm build/*.o
sudo rm build/*.bin
nasm -f bin -o ./build/bootloader.bin ./src/bootloader.asm
# nasm -f elf32 -o ./build/bootloader.o ./src/bootloader.asm
# nasm -f bin -o ./build/os.bin ./src/os.asm

echo Now assembling, compiling, and linking your kernel:
nasm -f elf32 -o ./build/os.o ./src/os.asm
# Remember this spot here: We will add 'gcc' commands here to compile C sources
gcc -m32 -Wall -O -fno-stack-protector -fno-pie -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdlib -nostdinc -fno-builtin -g -I./ -c -o ./build/main.o ./src/main.c

gcc -m32 -Wall -O -fno-stack-protector -fno-pie -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdlib -nostdinc -fno-builtin -g -I./ -c -o ./build/scrn.o ./src/scrn.c

gcc -m32 -Wall -O -fno-stack-protector -fno-pie -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdlib -nostdinc -fno-builtin -g -I./ -c -o ./build/gdt.o ./src/gdt.c

gcc -m32 -Wall -O -fno-stack-protector -fno-pie -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdlib -nostdinc -fno-builtin -g -I./ -c -o ./build/idt.o ./src/idt.c

gcc -m32 -Wall -O -fno-stack-protector -fno-pie -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdlib -nostdinc -fno-builtin -g -I./ -c -o ./build/isrs.o ./src/isrs.c

gcc -m32 -Wall -O -fno-stack-protector -fno-pie -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdlib -nostdinc -fno-builtin -g -I./ -c -o ./build/irq.o ./src/irq.c

gcc -m32 -Wall -O -fno-stack-protector -fno-pie -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdlib -nostdinc -fno-builtin -g -I./ -c -o ./build/timer.o ./src/timer.c

gcc -m32 -Wall -O -fno-stack-protector -fno-pie -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdlib -nostdinc -fno-builtin -g -I./ -c -o ./build/kb.o ./src/kb.c

# This links all your files. Remember that as you add *.o files, you need to
# add them after start.o. If you don't add them at all, they won't be in your kernel!
ld -m elf_i386 -oelf32-i386 -T ./src/objlink.ld -r -o ./build/kernel.o ./build/os.o ./build/main.o ./build/scrn.o ./build/gdt.o ./build/idt.o ./build/isrs.o ./build/irq.o ./build/timer.o ./build/kb.o
ld -m elf_i386 -T ./src/link.ld -o ./build/kernel.bin ./build/os.o ./build/main.o ./build/scrn.o ./build/gdt.o ./build/idt.o ./build/isrs.o ./build/irq.o ./build/timer.o ./build/kb.o
xxd ./build/kernel.bin >> ./build/kernel.txt
xxd ./build/kernel.o >> ./build/kernelobj.txt
xxd ./build/os.o >> ./build/os.txt
xxd ./build/main.o >> ./build/main.txt

dd if=./build/bootloader.bin of=./build/boot.img bs=512 count=2880

sudo mkdir ./build/fd
sudo mount -o loop ./build/boot.img ./build/fd
sudo cp ./build/kernel.bin ./build/fd
sudo umount ./build/fd
sudo rm -r ./build/fd
xxd ./build/boot.img > ./build/boot.txt
