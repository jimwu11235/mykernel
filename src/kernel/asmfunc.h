#ifndef _ASMFUNC_H
#define _ASMFUNC_H

#include "typedefine.h"

extern void io_hlt(void);
extern void io_cli(void);
extern void io_sti(void);
extern t_U32 io_load_eflags(void);
extern void io_store_eflags(t_U32 eflags);
extern t_U32 load_cr0(void);
extern void store_cr0(t_U32 cr0);
extern void load_tr(t_U32 tr);

extern unsigned int memtest_asm(t_U32 start, t_U32 end);

extern void task_switch_4(void);
extern void task_switch_3(void);

#endif