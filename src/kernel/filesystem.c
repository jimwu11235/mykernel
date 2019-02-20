#include "system.h"

void cmd_dir()
{
    FILEINFO_STRUCT *finfo = (FILEINFO_STRUCT *)(0x100000 + 0x2600);
    u16 i, j;
    u8 s[13];
    for(i = 0; i < 224; i++)
    {
        if(finfo[i].name[0] == 0x00)
        {
            break;
        }
        if(finfo[i].name[0] != 0xE5)
        {
            if((finfo[0].type & 0x18) == 0)
            {
                for(j = 0; j < 8; j++)
                {
                    s[j] = finfo[i].name[j];
                }
                s[8] = '.';
                s[9] = finfo[i].ext[0];
                s[10] = finfo[i].ext[1];
                s[11] = finfo[i].ext[2];
                s[12] = 0;
                printk("\nname: %s  ", s);
                printk("size: %d B  ", finfo[i].size);
                printk("no: %x", finfo[i].clustno);
            }
        }
    }
}

void cmd_cat(u8 *filename)
{
    FILEINFO_STRUCT *finfo = (FILEINFO_STRUCT *)(0x100000 + 0x2600);
    u16 i, j;
    u8 filefounded = 1;
    for(i = 0; i < 224; i++)
    {
        if(finfo[i].name[0] == 0x00)
        {
            break;
        }
        if(finfo[i].name[0] != 0xE5)
        {
            if((finfo[0].type & 0x18) == 0)
            {
                filefounded = 1;
                for(j = 0; j < 11; j++)
                {
                    // printk("\n%s %s  %d", finfo[i].name, filename, filefounded);
                    if(j < 8)
                    {
                        if((finfo[i].name[j] != filename[j]) && (finfo[i].name[j] != filename[j] - 0x20))
                        {
                            filefounded = 0;
                        }
                    }
                    else
                    {
                        if((finfo[i].name[j] != filename[j + 1]) && (finfo[i].name[j] != filename[j + 1] - 0x20))
                        {
                            filefounded = 0;
                        }
                    }
                }
            }
        }
        if(filefounded)
        {
            break;
        }
    }

    if(filefounded)
    {
        u8 *fptr = (u8 *)(0x100000 + 0x3e00 + finfo[i].clustno * 512);
        printk("\nfile: %s founded.\n", finfo[i].name);
        for(j = 0; j < finfo[i].size; j++)
        {
            printk("%c", fptr[j]);
        }
    }
    else
    {
        printk("\nfile not found");
    }
}