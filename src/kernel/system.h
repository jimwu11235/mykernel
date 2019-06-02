#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "typedefine.h"
#include "asmfunc.h"

/* SYSTEM */
struct regs /* This defines what the stack looks like after an ISR was running */
{
    t_U32 gs, fs, es, ds;      /* pushed the segs last */
    t_U32 edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    t_U32 int_no, err_code;    /* our 'push byte #' and ecodes do this */
    t_U32 eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

typedef struct _ADDRESS_DATA_STRUCT
{
    t_U32 kernel_start;
    t_U32 kernel_end;
    t_U32 start_entry;
    t_U32 rsvd;
} ADDRESS_DATA_STRUCT;

extern ADDRESS_DATA_STRUCT address_data;

/* FIFO32.C */
typedef struct _FIFO32_STRUCT
{
    t_S32 *buf;
    t_U32 write_entry;
    t_U32 read_entry;
    t_U32 size;
    t_U32 free;
    t_U8  flags;
}FIFO32_STRUCT;
extern void fifo32_init(FIFO32_STRUCT *fifo, t_U32 size, t_S32 *buf);
extern t_S8 fifo32_put(FIFO32_STRUCT *fifo, t_S32 data);
extern t_S32 fifo32_get(FIFO32_STRUCT *fifo);
extern t_U32 fifo32_status(FIFO32_STRUCT *fifo);

/* DEBUG.C */
typedef __builtin_va_list va_list;
#define va_start(ap, last)  (__builtin_va_start(ap, last))
#define va_arg(ap, type)    (__builtin_va_arg(ap, type))
#define va_end(ap)

extern void printk(const char *fmt, ...);

/* STRING.C */
extern t_U8 *memcpy(t_U8 *dest, const t_U8 *src, t_S32 count);
extern t_U8 *memset(t_U8 *dest, t_U8 val, t_S32 count);
extern t_U16 *memsetw(t_U16 *dest, t_U16 val, t_S32 count);
extern t_S32 strlen(const t_U8 *str);
extern t_U8 inportb (t_U16 _port);
extern void outportb (t_U16 _port, t_U8 _data);

/* SCRN.C */
extern void cls();
extern void putch(t_U8 c);
extern void puts(t_U8 *str);
extern void settextcolor(t_U8 forecolor, t_U8 backcolor);
extern void init_video();

/* GDT.C */
#define DEF_ACCESS_UNUSED           0x00
#define DEF_ACCESS_SYS_DATA32_RW    0x92
#define DEF_ACCESS_SYS_CODE32_RE    0x9A
#define DEF_ACCESS_APP_DATA32_RW    0xF2
#define DEF_ACCESS_APP_CODE32_RE    0xFA
#define DEF_ACCESS_SYS_TSS32        0x89

#define DEF_GRAN32_1B     0x4F
#define DEF_GRAN32_4KB    0xCF
#define DEF_GRAN16_1B     0x0F
#define DEF_GRAN16_4KB    0x8F
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
#define DEF_TIMER_POOL_SIZE     500

typedef void (*t_PtFn_TimerCallback)(void);

typedef enum _t_En_TimerStatus
{
    EN_TIMER_STATUS_FREE,
    EN_TIMER_STATUS_ALLOC,
    EN_TIMER_STATUS_USING
} t_En_TimerStatus;

typedef struct _t_St_TimerInfo
{
    struct _t_St_TimerInfo  *v_PtSt_NextTimeoutTimer;
    t_U32                   v_U32_Timeout;
    t_En_TimerStatus        v_En_TimerStatus;
    t_PtFn_TimerCallback    v_PtFn_TimerCallback;
} t_St_TimerInfo;

typedef struct _t_St_TimerMan
{
    t_U32           v_U32_TimeCounter;
    t_U32           v_U32_NextTimeoutTime;
    t_St_TimerInfo  *v_PtSt_FirstTimeoutTimer;
    t_St_TimerInfo  v_PtSt_TimmerPool[DEF_TIMER_POOL_SIZE];
} t_St_TimerMan;

extern void timer_phase(int hz);
extern void timer_wait(int ticks);
extern void timer_install();

extern void f_Vd_TimerAlloc(t_St_TimerInfo **v_PtSt_TimerInfo);
extern void f_Vd_TimerFree(t_St_TimerInfo *v_PtSt_TimerInfo);
extern void f_Vd_TimerInit(t_St_TimerInfo *v_PtSt_TimerInfo, t_PtFn_TimerCallback v_PtFnCallback);
extern void f_Vd_TimerSetting(t_St_TimerInfo *v_PtSt_TimerInfo, t_U32 v_U32_Timeout);

/* KEYBOARD.C */
extern void keyboard_install(FIFO32_STRUCT *fifo);

/* MEMORY.C */
#define DEF_FREE_MEMORY_POOL_SIZE 4096
typedef struct _t_St_MemoryInfo
{
    t_U32 v_U32_Address;
    t_U32 v_U32_Size;
} t_St_MemoryInfo;

typedef struct _t_St_MemoryMan
{
    t_U32           v_U32_CurrentFreeNum;
    t_U32           v_U32_MaxFreeNum;
    t_U32           v_U32_LostSize;
    t_U32           v_U32_LostNum;
    t_St_MemoryInfo v_PtSt_FreeMemoryPool[DEF_FREE_MEMORY_POOL_SIZE];
} t_St_MemoryMan;

extern t_U32 f_U32_MemoryTest(t_U32 v_U32_StartAddress, t_U32 v_U32_EndAddress);
extern void f_Vd_MemoryManInit(t_St_MemoryMan *v_PtSt_MemoryMan);
extern t_U32 f_U32_GetFreeMemorySize(t_St_MemoryMan *v_PtSt_MemoryMan);
extern t_U32 f_U32_MemoryAlloc(t_St_MemoryMan *v_PtSt_MemoryMan, t_U32 v_U32_MemorySize);
extern t_U32 f_U32_MemoryAlloc4k(t_St_MemoryMan *v_PtSt_MemoryMan, t_U32 v_U32_MemorySize);
extern t_S8 f_S8_MemoryFree(t_St_MemoryMan *v_PtSt_MemoryMan, t_U32 v_U32_MemoryAddress, t_U32 v_U32_MemorySize);
extern t_S8 f_S8_MemoryFree4k(t_St_MemoryMan *v_PtSt_MemoryMan, t_U32 v_U32_MemoryAddress, t_U32 v_U32_MemorySize);

/* FILESYSTEM.C */
typedef struct _FILEINFO_STRUCT
{
    t_U32 rsvd0[8];
    t_U8 name[8], ext[3], type;
    t_U8 rsvd1[10];
    t_U16 time, date, clustno;
    t_U32 size;
}FILEINFO_STRUCT;
extern void cmd_dir();
extern void cmd_cat(t_U8 *filename);

/* MULTITASKING.H */
typedef struct _t_St_Tss32
{
    t_U32   v_U32_BackLink;
    t_U32   v_U32_Esp0;
    t_U32   v_U32_Ss0;
    t_U32   v_U32_Esp1;
    t_U32   v_U32_Ss1;
    t_U32   v_U32_Esp2;
    t_U32   v_U32_Ss2;
    t_U32   v_U32_Cr3;

    /* 32-bit register */
    t_U32   v_U32_Eip;
    t_U32   v_U32_Eflags;
    t_U32   v_U32_Eax;
    t_U32   v_U32_Ecx;
    t_U32   v_U32_Edx;
    t_U32   v_U32_Ebx;
    t_U32   v_U32_Esp;
    t_U32   v_U32_Ebp;
    t_U32   v_U32_Esi;
    t_U32   v_U32_Edi;

    /* 16-bit register */
    t_U32   v_U32_Es;
    t_U32   v_U32_Cs;
    t_U32   v_U32_Ss;
    t_U32   v_U32_Ds;
    t_U32   v_U32_Fs;
    t_U32   v_U32_Gs;

    t_U32   v_U32_Ldtr;
    t_U32   v_U32_IoMap;
} t_St_Tss32;

#endif