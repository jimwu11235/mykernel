#include "system.h"

void cmd_dir()
{
    FILEINFO_STRUCT *finfo = (FILEINFO_STRUCT *)(0x100000 + 0x2600);
    int i, j;
    char s[13];
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
                putch('\n');
                puts(s);
                printk("  size: %d B", finfo[i].size);
            }
        }
    }
}