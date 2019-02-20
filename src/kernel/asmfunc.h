#ifndef _ASMFUNC_H
#define _ASMFUNC_H

#include "typedefine.h"

extern u32 io_load_eflags(void);
extern void io_store_eflags(u32 eflags);
extern u32 load_cr0(void);
extern void store_cr0(u32 cr0);

extern unsigned int memtest_asm(u32 start, u32 end);

#endif