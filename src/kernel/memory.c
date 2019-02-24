#include "system.h"

#define EFLAGS_AC_BIT       0x00040000
#define CR0_CACHE_DISABLE   0x60000000

t_U32 f_U32_MemoryTest(t_U32 v_U32_StartAddress, t_U32 v_U32_EndAddress)
{
    t_U32 eflags, cr0;
    t_U8 flg486 = 0;
    t_U32 memnum;

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

    memnum = memtest_asm(v_U32_StartAddress, v_U32_EndAddress);

    /* Restore cache */
    if(flg486)
    {
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }
    printk("\nmemnum = %dMB", memnum / (1024 * 1024));

    return memnum;
}


void f_Vd_MemoryManInit(t_St_MemoryMan *v_PtSt_MemoryMan)
{
    v_PtSt_MemoryMan->v_U32_CurrentFreeNum = 0;
    v_PtSt_MemoryMan->v_U32_MaxFreeNum = 0;
    v_PtSt_MemoryMan->v_U32_LostSize = 0;
    v_PtSt_MemoryMan->v_U32_LostNum = 0;
}


t_U32 f_U32_GetFreeMemorySize(t_St_MemoryMan *v_PtSt_MemoryMan)
{
    t_U32 v_U32_I = 0;
    t_U32 v_U32_FreeMemorySize = 0;

    for(v_U32_I = 0; v_U32_I < v_PtSt_MemoryMan->v_U32_CurrentFreeNum; v_PtSt_MemoryMan++)
    {
        v_U32_FreeMemorySize += v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Size;
    }

    return v_U32_FreeMemorySize;
}


t_U32 f_U32_MemoryAlloc(t_St_MemoryMan *v_PtSt_MemoryMan, t_U32 v_U32_MemorySize)
{
    t_U32 v_U32_I = 0;
    t_U32 v_U32_AllocMemoryAddress = 0;

    for(v_U32_I = 0; v_U32_I < v_PtSt_MemoryMan->v_U32_CurrentFreeNum; v_PtSt_MemoryMan++)
    {
        /* Find the first memory block that is big enough to allocate required memory size */
        if(v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Size >= v_U32_MemorySize)
        {
            v_U32_AllocMemoryAddress = v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Address;

            /* Update allocated memory block information */
            v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Address += v_U32_MemorySize;
            v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Size -= v_U32_MemorySize;

            /* Delete memory block if the memory block is empty */
            if(v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Size == 0)
            {
                v_PtSt_MemoryMan->v_U32_CurrentFreeNum--;
                for(; v_U32_I < v_PtSt_MemoryMan->v_U32_CurrentFreeNum; v_U32_I++)
                {
                    v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I] = v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I + 1];
                }
            }
            return v_U32_AllocMemoryAddress;
        }
    }

    /* Return 0 if memory allocation is failed */
    return v_U32_AllocMemoryAddress;
}


t_U32 f_U32_MemoryAlloc4k(t_St_MemoryMan *v_PtSt_MemoryMan, t_U32 v_U32_MemorySize)
{
    v_U32_MemorySize = (v_U32_MemorySize + 0xFFF) & 0xFFFFF000;
    return f_U32_MemoryAlloc(v_PtSt_MemoryMan, v_U32_MemorySize);
}


t_S8 f_S8_MemoryFree(t_St_MemoryMan *v_PtSt_MemoryMan, t_U32 v_U32_MemoryAddress, t_U32 v_U32_MemorySize)
{
    t_U32 v_U32_I = 0;
    t_U32 v_U32_J = 0;

    /* Check the location that freed memory should be put */
    for(v_U32_I = 0; v_U32_I < v_PtSt_MemoryMan->v_U32_CurrentFreeNum; v_U32_I++)
    {
        if(v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Address > v_U32_MemoryAddress)
        {
            break;
        }
    }

    /* Case 1: the freed memory can be merged with previous and/or following memory blocks */
    if(v_U32_I > 0)
    {
        /* Check if the freed memory can be merged with previous memory block */
        if(v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I - 1].v_U32_Address + v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Size == v_U32_MemoryAddress)
        {
            v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I - 1].v_U32_Size += v_U32_MemorySize;

            if(v_U32_I < v_PtSt_MemoryMan->v_U32_CurrentFreeNum)
            {
                /* Check if the freed memory can be merged with following memory block */
                if(v_U32_MemoryAddress + v_U32_MemorySize == v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Address)
                {
                    v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I - 1].v_U32_Size += v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Size;

                    /* Delete merged memory block */
                    v_PtSt_MemoryMan->v_U32_CurrentFreeNum--;
                    for(; v_U32_I < v_PtSt_MemoryMan->v_U32_CurrentFreeNum; v_U32_I++)
                    {
                        v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I] = v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I + 1];
                    }
                }
            }
            return 0;
        }
    }

    /* Case 2: the freed memory can only be merged with following memory blocks */
    if(v_U32_I < v_PtSt_MemoryMan->v_U32_CurrentFreeNum)
    {
        if(v_U32_MemoryAddress + v_U32_MemorySize == v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Address)
        {
            v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Address = v_U32_MemoryAddress;
            v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Size += v_U32_MemorySize;
            return 0;
        }
    }

    /* Case 3: the freed memory can not be merged */
    if(v_PtSt_MemoryMan->v_U32_CurrentFreeNum < DEF_FREE_MEMORY_POOL_SIZE)
    {
        for(v_U32_J = v_PtSt_MemoryMan->v_U32_CurrentFreeNum; v_U32_J > v_U32_I; v_U32_J--)
        {
            v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_J] = v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_J - 1];
        }

        v_PtSt_MemoryMan->v_U32_CurrentFreeNum++;

        if(v_PtSt_MemoryMan->v_U32_MaxFreeNum < v_PtSt_MemoryMan->v_U32_CurrentFreeNum)
        {
            v_PtSt_MemoryMan->v_U32_MaxFreeNum = v_PtSt_MemoryMan->v_U32_CurrentFreeNum;
        }

        v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Address = v_U32_MemoryAddress;
        v_PtSt_MemoryMan->v_PtSt_FreeMemoryPool[v_U32_I].v_U32_Size = v_U32_MemorySize;
        return 0;
    }

    /* Case 4: free memory failed due to the free memory pool size is not enough */
    v_PtSt_MemoryMan->v_U32_LostNum++;
    v_PtSt_MemoryMan->v_U32_LostSize += v_U32_MemorySize;
    return -1;
}


t_S8 f_S8_MemoryFree4k(t_St_MemoryMan *v_PtSt_MemoryMan, t_U32 v_U32_MemoryAddress, t_U32 v_U32_MemorySize)
{
    v_U32_MemorySize = (v_U32_MemorySize + 0xFFF) & 0xFFFFF000;
    return f_S8_MemoryFree(v_PtSt_MemoryMan, v_U32_MemoryAddress, v_U32_MemorySize);
}