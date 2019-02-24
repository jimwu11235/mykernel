#include "system.h"

void fifo32_init(FIFO32_STRUCT *fifo, t_U32 size, t_S32 *buf)
{
    fifo->size = size;
    fifo->buf = buf;
    fifo->free = size;
    fifo->flags = 0;
    fifo->write_entry = 0;
    fifo->read_entry = 0;
}

t_S8 fifo32_put(FIFO32_STRUCT *fifo, t_S32 data)
{
    if(fifo->free == 0)
    {
        fifo->flags = 1;
        return -1;
    }
    fifo->buf[fifo->write_entry] = data;
    fifo->write_entry++;
    if(fifo->write_entry == fifo->size)
    {
        fifo->write_entry = 0;
    }
    fifo->free--;

    return 0;
}

t_S32 fifo32_get(FIFO32_STRUCT *fifo)
{
    t_S32 data;
    if(fifo->free == fifo->size)
    {
        return -1;
    }
    data = fifo->buf[fifo->read_entry];
    fifo->read_entry++;
    if(fifo->read_entry == fifo->size)
    {
        fifo->read_entry = 0;
    }
    fifo->free++;

    return data;
}

t_U32 fifo32_status(FIFO32_STRUCT *fifo)
{
    return fifo->size - fifo->free;
}