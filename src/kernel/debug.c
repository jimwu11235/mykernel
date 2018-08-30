#include "system.h"

void printk(const char *fmt, ...)
{
    va_list ap;
    int i;
    int base;
    int d, d_temp;
    unsigned int x, x_temp;
    int zero_flag;
    char c;
    
    va_start(ap, fmt);
    while(*fmt)
    {
        if(*fmt == '%')
        {
            switch(*(++fmt))
            {
                /* Print integer */
                case 'd':
                    d = va_arg(ap, int);
                    /* Check sign */
                    if(d & 0x80000000)
                    {
                        d = ~d + 1;
                        putch('-');
                    }

                    /* Parse to decimal */
                    base = 1000000000;
                    zero_flag = 0;
                    for( i = 0; i < 10; i++)
                    {
                        d_temp = (int)(d % base);
                        if((d - d_temp) != 0 || zero_flag != 0)
                        {
                            zero_flag = 1;
                            putch((unsigned char)((int)((d - d_temp) / base) + 0x30));
                        }
                        if(zero_flag == 0 && i == 9)
                        {
                            putch('0');
                        }
                        d = d_temp;
                        base = (int)(base / 10);
                    }
                    break;

                /* Print binary */
                case 'b':
                x = va_arg(ap, unsigned int);
                for(i = (sizeof(unsigned int) * 8) - 1; i >= 0; i--)
                {
                    if(x & (0x1 << i))
                    {
                        putch('1');
                    }
                    else
                    {
                        putch('0');
                    }

                    if((i % 4) == 0)
                    {
                        putch(' ');
                    }
                }
                break;

                /* Print hex */
                case 'x':
                    x = va_arg(ap, unsigned int);
                    puts("0x");
                    for(i = (sizeof(unsigned int) * 2) - 1; i >= 0; i--)
                    {
                        x_temp = (x & (0xF << (i * 4))) >> (i * 4);
                        if(x_temp < 10)
                        {
                            putch((unsigned char)(x_temp + 0x30));
                        }
                        else
                        {
                            putch((unsigned char)(x_temp + 0x37));
                        }
                    }
                    break;
                
                /* Print char */
                case 'c':
                    c = va_arg(ap, int);
                    putch(c);
                    break;
                    
                default:
                    c = *fmt;
                    putch(c);
            }
        }
        else
        {
            c = *fmt;
            putch(c);
        }
        ++fmt;
    }
    va_end(ap);
}