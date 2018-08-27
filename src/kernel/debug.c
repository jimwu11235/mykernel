#include "system.h"

void printk(const char *fmt, ...)
{
    va_list ap;
    int i;
    int base;
    int d, d_temp;
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
                        d = d_temp;
                        base = (int)(base / 10);
                    }

                    // putch((unsigned char)d + 0x30);
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