#ifndef _ASMFUNC_H
#define _ASMFUNC_H

#include "typedefine.h"

extern t_U32 io_load_eflags(void);
extern void io_store_eflags(t_U32 eflags);
extern t_U32 load_cr0(void);
extern void store_cr0(t_U32 cr0);

extern unsigned int memtest_asm(t_U32 start, t_U32 end);

#endif