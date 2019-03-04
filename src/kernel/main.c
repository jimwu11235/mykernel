#include "system.h"

t_St_TimerInfo *v_PtSt_Timer1;
t_St_TimerInfo *v_PtSt_Timer2;
t_St_TimerInfo *v_PtSt_Timer3;

void timerCallback1()
{
    printk("\nTimer1 timeout");
    f_Vd_TimerSetting(v_PtSt_Timer1, 100);
}

void timerCallback2()
{
    printk("\nTimer2 timeout");
    f_Vd_TimerSetting(v_PtSt_Timer2, 200);
}

void timerCallback3()
{
    printk("\nTimer3 timeout");
    f_Vd_TimerSetting(v_PtSt_Timer3, 400);
}


/* We will use this later on for reading from the I/O ports to get data
*  from devices such as the keyboard. We are using what is called
*  'inline assembly' in these routines to actually do the work */
unsigned char inportb (unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

/* We will use this to write to I/O ports to send bytes to devices. This
*  will be used in the next tutorial for changing the textmode cursor
*  position. Again, we use some inline assembly for the stuff that simply
*  cannot be done in C */
void outportb (unsigned short _port, unsigned char _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

/* This is a very simple main() function. All it does is sit in an
*  infinite loop. This will be like our 'idle' loop */
void main()
{
    /* You would add commands after here */
    FIFO32_STRUCT *fifo;
    t_S32 fifobuf[128];
    t_S32 fifodata;

    unsigned char text[] = "Hello World!\n";

    t_St_MemoryMan v_St_MemoryMan;

    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
    timer_install();

    fifo32_init(fifo, 128, fifobuf);

    keyboard_install(fifo);
    init_video();

    __asm__ __volatile__ ("sti");
    outportb(0x21, 0xf0);
    outportb(0xA1, 0xff);

    puts(text);
    printk("\nkernel_start = %x", address_data.kernel_start);
    printk("\nkernel_end = %x", address_data.kernel_end);
    printk("\nstart_entry = %x", address_data.start_entry);
    t_U32 v_U32_TotalMemory = f_U32_MemoryTest(address_data.kernel_end, 0xffffffff);
    f_Vd_MemoryManInit(&v_St_MemoryMan);
    f_S8_MemoryFree(&v_St_MemoryMan, address_data.kernel_end, v_U32_TotalMemory);
    printk("\nFree Memory Size = %dB", f_U32_GetFreeMemorySize(&v_St_MemoryMan));
    f_U32_MemoryAlloc4k(&v_St_MemoryMan, 0x1);
    printk("\nFree Memory Size = %dB", f_U32_GetFreeMemorySize(&v_St_MemoryMan));
    printk("\nFree Memory Size = %dMB", f_U32_GetFreeMemorySize(&v_St_MemoryMan) / (1024 * 1024));

    cmd_dir();
    cmd_cat("aaa     .txt");

    f_Vd_TimerAlloc(&v_PtSt_Timer1);
    f_Vd_TimerInit(v_PtSt_Timer1, timerCallback1);
    f_Vd_TimerSetting(v_PtSt_Timer1, 100);
    
    f_Vd_TimerAlloc(&v_PtSt_Timer2);
    f_Vd_TimerInit(v_PtSt_Timer2, timerCallback2);
    f_Vd_TimerSetting(v_PtSt_Timer2, 200);

    f_Vd_TimerAlloc(&v_PtSt_Timer3);
    f_Vd_TimerInit(v_PtSt_Timer3, timerCallback3);
    f_Vd_TimerSetting(v_PtSt_Timer3, 400);

    /* ...and leave this loop in. There is an endless loop in
    *  'start.asm' also, if you accidentally delete this next line */
    for (;;)
    {
        if(fifo32_status(fifo) != 0)
        {
            fifodata = fifo32_get(fifo);
            putch((t_U8)fifodata);
        }
    }
}