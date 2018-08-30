#include "system.h"

#define EFLAGS_AC_BIT       0x00040000
#define CR0_CACHE_DISABLE   0x60000000

unsigned int memtest(unsigned int start, unsigned int end)
{
    int eflags, cr0;
    int flg486 = 0;

    /* Check CPU version (386 or 486) */
    eflags = io_load_eflags();
    eflags |= EFLAGS_AC_BIT; /* AC bit = 1 */
    io_store_eflags(eflags);
    eflags = io_load_eflags();
    if((eflags & EFLAGS_AC_BIT) != 0) /* If 386, AC bit = 0 */
    {
        flg486 = 1;
    }
    eflags &= ~EFLAGS_AC_BIT;
    io_store_eflags(eflags);

    /* Turn off cache */
    if(flg486)
    {
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }

    if(flg486)
    {
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }

    printk("\nkernel_start = %d", address_data.kernel_start);
    printk("\nkernel_end = %d", address_data.kernel_end);
    printk("\nstart_entry = %d", address_data.start_entry);
    printk("\nkernel_start = %x", address_data.kernel_start);
    printk("\nkernel_end = %x", address_data.kernel_end);
    printk("\nstart_entry = %x", address_data.start_entry);
    printk("\naaa = %b", 0xFFFFFFFF);

    return eflags;
}