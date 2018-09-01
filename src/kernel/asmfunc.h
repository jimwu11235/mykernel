#ifndef _ASMFUNC_H
#define _ASMFUNC_H

extern int io_load_eflags(void);
extern void io_store_eflags(int eflags);
extern int load_cr0(void);
extern void store_cr0(int cr0);

extern unsigned int memtest_asm(unsigned int start, unsigned int end);

#endif