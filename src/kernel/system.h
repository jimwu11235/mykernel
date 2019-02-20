#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "typedefine.h"
#include "asmfunc.h"

/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

/* FIFO32.C */
typedef struct _FIFO32_STRUCT
{
    s32 *buf;
    u32 write_entry;
    u32 read_entry;
    u32 size;
    u32 free;
    u8  flags;
}FIFO32_STRUCT;
extern void fifo32_init(FIFO32_STRUCT *fifo, u32 size, s32 *buf);
extern s8 fifo32_put(FIFO32_STRUCT *fifo, s32 data);
extern s32 fifo32_get(FIFO32_STRUCT *fifo);
extern u32 fifo32_status(FIFO32_STRUCT *fifo);

/* DEBUG.C */
typedef __builtin_va_list va_list;
#define va_start(ap, last)  (__builtin_va_start(ap, last))
#define va_arg(ap, type)    (__builtin_va_arg(ap, type))
#define va_end(ap)

extern void printk(const char *fmt, ...);

/* STRING.C */
extern u8 *memcpy(u8 *dest, const u8 *src, s32 count);
extern u8 *memset(u8 *dest, u8 val, s32 count);
extern u16 *memsetw(u16 *dest, u16 val, s32 count);
extern s32 strlen(const u8 *str);
extern u8 inportb (u16 _port);
extern void outportb (u16 _port, u8 _data);

/* SCRN.C */
extern void cls();
extern void putch(u8 c);
extern void puts(u8 *str);
extern void settextcolor(u8 forecolor, u8 backcolor);
extern void init_video();

/* GDT.C */
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void gdt_install();

/* IDT.C */
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install();

/* ISRS.C */
extern void isrs_install();

/* IRQ.C */
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);
extern void irq_install();

/* TIMER.C */
extern void timer_phase(int hz);
extern void timer_wait(int ticks);
extern void timer_install();

/* KEYBOARD.C */
extern void keyboard_install(FIFO32_STRUCT *fifo);

/* MEMORY.C */
typedef struct _ADDRESS_DATA_STRUCT
{
    u32 kernel_start;
    u32 kernel_end;
    u32 start_entry;
    u32 rsvd;
}ADDRESS_DATA_STRUCT;
extern ADDRESS_DATA_STRUCT address_data;
extern u32 memtest(u32 start, u32 end);

/* FILESYSTEM.C */
typedef struct _FILEINFO_STRUCT
{
    u32 rsvd0[8];
    u8 name[8], ext[3], type;
    u8 rsvd1[10];
    u16 time, date, clustno;
    u32 size;
}FILEINFO_STRUCT;
extern void cmd_dir();
extern void cmd_cat(u8 *filename);

#endif