#include "system.h"

t_U8 *memcpy(t_U8 *dest, const t_U8 *src, t_S32 count)
{
    const t_U8 *sp = (const t_U8 *)src;
    t_U8 *dp = (t_U8 *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

t_U8 *memset(t_U8 *dest, t_U8 val, t_S32 count)
{
    t_U8 *temp = (t_U8 *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

t_U16 *memsetw(t_U16 *dest, t_U16 val, t_S32 count)
{
    t_U16 *temp = (t_U16 *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

t_S32 strlen(const t_U8 *str)
{
    t_S32 retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}