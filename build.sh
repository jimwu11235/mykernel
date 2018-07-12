nasm -f bin -o ./build/bootloader.bin ./src/bootloader.asm
nasm -f bin -o ./build/os.bin ./src/os.asm
dd if=./build/bootloader.bin of=./build/boot.img bs=512 count=2880

sudo mkdir ./build/floppy
sudo mount -o loop ./build/boot.img ./build/floppy
sudo cp ./build/os.bin ./build/floppy
sudo umount ./build/floppy
sudo rm -r ./build/floppy
xxd ./build/boot.img > ./build/boot.txt
