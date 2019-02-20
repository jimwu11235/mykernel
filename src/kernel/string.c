#include "system.h"

u8 *memcpy(u8 *dest, const u8 *src, s32 count)
{
    const u8 *sp = (const u8 *)src;
    u8 *dp = (u8 *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

u8 *memset(u8 *dest, u8 val, s32 count)
{
    u8 *temp = (u8 *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

u16 *memsetw(u16 *dest, u16 val, s32 count)
{
    u16 *temp = (u16 *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

s32 strlen(const u8 *str)
{
    s32 retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}