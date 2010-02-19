#ifndef _FIFO_H_
#define _FIFO_H_

#include "os.h"

typedef struct fifo_struct
{
    int     buffer[FIFOSIZE];  /* message circular buffer */
    int     fillCount;         /* keeps track of the number of items */          
    int     next;              /* next spot to write in */
    int     first;             /* first element to read */

} fifo_struct;

fifo_struct arrFifos[MAXFIFO];
static int numFifos = 0; /* keeps track of the number of fifos */

FIFO OS_InitFiFo()
{
    OS_DI();
    /* gets next available message queue */
    if ( numFifos < MAXFIFO )
    {
        arrFifos[numFifos].fillCount = 0;
        arrFifos[numFifos].next = 0;
        arrFifos[numFifos].first = 0;
        ++numFifos;
        return numFifos;
    }
    else
    {
        /* ran out of message queues */
        return INVALIDFIFO;
    }
    OS_EI();
}

void OS_Write(FIFO f, int val)
{
    OS_DI();
    int idx = f - 1;
    fifo_struct *curFifo = &arrFifos[idx];

    /* the buffer still has space to write
       if it is full writes are ignored */
    if ( curFifo->fillCount < FIFOSIZE )
    {
        curFifo->buffer[curFifo->next] = val;
        curFifo->next = (curFifo->next + 1) % FIFOSIZE;
    }

    /* increment fillcount if not full */
    curFifo->fillCount = (curFifo->fillCount == FIFOSIZE) ? FIFOSIZE : ++curFifo->fillCount;

    OS_EI();
}

BOOL OS_Read(FIFO f, int *val)
{
    OS_DI();
    int idx = f - 1;
    fifo_struct *curFifo = &arrFifos[idx];

    /* the buffer is empty */
    if ( curFifo->fillCount <= 0 )
    {
        return FALSE;
    }
    /* there are still elements in the buffer */
    else
    {
        *val = curFifo->buffer[curFifo->first];
        curFifo->first = (curFifo->first + 1) % FIFOSIZE;
        curFifo->fillCount--;
        return TRUE;
    }
    OS_EI();
}

#endif /* _FIFO_H_ */
