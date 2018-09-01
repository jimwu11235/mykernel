#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *fp_img;
    FILE *fp_bootloader;
    unsigned char buff[512];
    unsigned char c;
    fp_bootloader = fopen(argv[1], "rb");
    fp_img = fopen(argv[2], "rb+");
    /* read first sector from bootloader.bin */
    fread(buff, 1, 512, fp_bootloader);

    /* replace jmp address */
    fseek(fp_img, 1, SEEK_SET);
    fputc(0x4E, fp_img);

    /* replace bootloader code */
    fseek(fp_img, 62, SEEK_SET);
    fwrite(&buff[62], 1, 450, fp_img);

    fclose(fp_bootloader);
    fclose(fp_img);
    
    return 0;
}