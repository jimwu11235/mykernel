#include "system.h"

#define EFLAGS_AC_BIT       0x00040000
#define CR0_CACHE_DISABLE   0x60000000

u32 memtest(u32 start, u32 end)
{
    u32 eflags, cr0;
    u8 flg486 = 0;
    u32 memnum;

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

    memnum = memtest_asm(address_data.kernel_end, 0xffffffff);

    if(flg486)
    {
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }

    printk("\nkernel_start = %x", address_data.kernel_start);
    printk("\nkernel_end = %x", address_data.kernel_end);
    printk("\nstart_entry = %x", address_data.start_entry);
    printk("\nmemnum = %dMB", memnum / (1024 * 1024));

    return eflags;
}