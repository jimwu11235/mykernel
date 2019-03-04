#include "system.h"

static t_St_TimerMan    v_FsSt_TimerMan;
static t_S32            v_FsS32_CbkFifo[DEF_TIMER_POOL_SIZE];
static FIFO32_STRUCT    v_FsSt_CbkFifoMan;

/* This will keep track of how many ticks that the system
*  has been running for */
int timer_ticks = 0;

void timer_phase(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    outportb(0x43, 0x36);             /* Set our command byte 0x36 */
    outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

/* Handles the timer. In this case, it's very simple: We
*  increment the 'timer_ticks' variable every time the
*  timer fires. By default, the timer fires 18.222 times
*  per second. Why 18.222Hz? Some engineer at IBM must've
*  been smoking something funky */
void timer_handler(struct regs *r)
{
    t_St_TimerInfo          *v_PtSt_TimerTemp;
    t_PtFn_TimerCallback    v_PtFn_TimerCallback;
    t_U32                   v_U32_CbkFifoCounter;

    v_FsSt_TimerMan.v_U32_TimeCounter++;
    
    /* Check if the time counter reach the next timeout time */
    if(v_FsSt_TimerMan.v_U32_TimeCounter >= v_FsSt_TimerMan.v_U32_NextTimeoutTime)
    {
        v_PtSt_TimerTemp = v_FsSt_TimerMan.v_PtSt_FirstTimeoutTimer;
        v_U32_CbkFifoCounter = 0;
        while(1)
        {
            if(v_PtSt_TimerTemp->v_U32_Timeout <= v_FsSt_TimerMan.v_U32_TimeCounter)
            {
                /* Timer timeout */
                v_PtSt_TimerTemp->v_En_TimerStatus = EN_TIMER_STATUS_ALLOC;
                // v_PtSt_TimerTemp->v_PtFn_TimerCallback();
                fifo32_put(&v_FsSt_CbkFifoMan, v_PtSt_TimerTemp->v_PtFn_TimerCallback);
                v_PtSt_TimerTemp = v_PtSt_TimerTemp->v_PtSt_NextTimeoutTimer;
                v_U32_CbkFifoCounter++;
            }
            else
            {
                break;
            }
        }
        v_FsSt_TimerMan.v_PtSt_FirstTimeoutTimer = v_PtSt_TimerTemp;
        v_FsSt_TimerMan.v_U32_NextTimeoutTime = v_PtSt_TimerTemp->v_U32_Timeout;

        /* Execute callback function */
        while(v_U32_CbkFifoCounter--)
        {
            v_PtFn_TimerCallback = (t_PtFn_TimerCallback)fifo32_get(&v_FsSt_CbkFifoMan);
            v_PtFn_TimerCallback();
        }
    }
}

/* This will continuously loop until the given time has
*  been reached */
void timer_wait(int ticks)
{
    unsigned long eticks;

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
}

/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
void timer_install()
{
    timer_phase(100);

    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);

    /* Set sentinel timer */
    t_St_TimerInfo *v_PtSt_SentinelTimer;
    f_Vd_TimerAlloc(&v_PtSt_SentinelTimer);
    v_PtSt_SentinelTimer->v_U32_Timeout = 0xFFFFFFFF;
    v_PtSt_SentinelTimer->v_En_TimerStatus = EN_TIMER_STATUS_ALLOC;
    v_PtSt_SentinelTimer->v_PtSt_NextTimeoutTimer = 0;
    v_FsSt_TimerMan.v_PtSt_FirstTimeoutTimer = v_PtSt_SentinelTimer;
    v_FsSt_TimerMan.v_U32_NextTimeoutTime = 0xFFFFFFFF;

    /* Init callback function fifo */
    fifo32_init(&v_FsSt_CbkFifoMan, DEF_TIMER_POOL_SIZE, v_FsS32_CbkFifo);
}


void f_Vd_TimerAlloc(t_St_TimerInfo **v_PtSt_TimerInfo)
{
    t_U32 v_U32_I;

    *v_PtSt_TimerInfo = 0;
    for(v_U32_I = 0; v_U32_I < DEF_TIMER_POOL_SIZE; v_U32_I++)
    {
        if(v_FsSt_TimerMan.v_PtSt_TimmerPool[v_U32_I].v_En_TimerStatus == EN_TIMER_STATUS_FREE)
        {
            v_FsSt_TimerMan.v_PtSt_TimmerPool[v_U32_I].v_En_TimerStatus = EN_TIMER_STATUS_ALLOC;
            *v_PtSt_TimerInfo = &v_FsSt_TimerMan.v_PtSt_TimmerPool[v_U32_I];
            break;
        }
    }
}


void f_Vd_TimerFree(t_St_TimerInfo *v_PtSt_TimerInfo)
{
    v_PtSt_TimerInfo->v_En_TimerStatus = EN_TIMER_STATUS_FREE;
}


void f_Vd_TimerInit(t_St_TimerInfo *v_PtSt_TimerInfo, t_PtFn_TimerCallback v_PtFnCallback)
{
    v_PtSt_TimerInfo->v_PtFn_TimerCallback = v_PtFnCallback;
}


void f_Vd_TimerSetting(t_St_TimerInfo *v_PtSt_TimerInfo, t_U32 v_U32_Timeout)
{
    t_St_TimerInfo *v_PtSt_TimerTemp1;
    t_St_TimerInfo *v_PtSt_TimerTemp2;
    t_U32 v_U32_Eflags;

    /* Disable system interrupt */
    v_U32_Eflags = io_load_eflags();
    io_cli();
    
    v_PtSt_TimerInfo->v_U32_Timeout = v_FsSt_TimerMan.v_U32_TimeCounter + v_U32_Timeout;
    v_PtSt_TimerInfo->v_En_TimerStatus = EN_TIMER_STATUS_USING;

    /* Check if the timeout time of new timer is smaller than any others */
    if(v_PtSt_TimerInfo->v_U32_Timeout <= v_FsSt_TimerMan.v_PtSt_FirstTimeoutTimer->v_U32_Timeout)
    {
        v_PtSt_TimerInfo->v_PtSt_NextTimeoutTimer = v_FsSt_TimerMan.v_PtSt_FirstTimeoutTimer;
        v_FsSt_TimerMan.v_PtSt_FirstTimeoutTimer = v_PtSt_TimerInfo;
        v_FsSt_TimerMan.v_U32_NextTimeoutTime = v_PtSt_TimerInfo->v_U32_Timeout;
    }
    else
    {
        v_PtSt_TimerTemp1 = v_FsSt_TimerMan.v_PtSt_FirstTimeoutTimer;
        while(1)
        {
            v_PtSt_TimerTemp2 = v_PtSt_TimerTemp1;
            v_PtSt_TimerTemp1 = v_PtSt_TimerTemp1->v_PtSt_NextTimeoutTimer;
            if(v_PtSt_TimerInfo->v_U32_Timeout <= v_PtSt_TimerTemp1->v_U32_Timeout)
            {
                v_PtSt_TimerTemp2->v_PtSt_NextTimeoutTimer = v_PtSt_TimerInfo;
                v_PtSt_TimerInfo->v_PtSt_NextTimeoutTimer = v_PtSt_TimerTemp1;
                break;
            }
        }
    }
    io_store_eflags(v_U32_Eflags);
}
