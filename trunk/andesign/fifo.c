/*******************************************************************
*
*   INTERPROCESS COMMUNICATION:
*   FIFOs are first-in-first-out bounded buffers. Elements are read in the same
*   order as they were written. When writes overtake reads, the first unread
*   element will be dropped. Thus, ordering is always preserved.
*   "read" and "write" on FIFOs are atomic, i.e., they are indivisible, and
*   they are non-blocking. All FIFOs are of the same size. All data elements
*   are assumed to be unsigned int.
* 
*   AUTHOR: Ed
* 
*   DATE:2/19/2010
*
*******************************************************************/

//#include "os.h"
//#include "fifo.h"
//#include <stdlib.h>

//typedef struct fifo_struct
//{
//    int     buffer[FIFOSIZE];  /* message circular buffer */
//    int     fillCount;         /* keeps track of the number of items */
//    int     next;              /* next spot to write in */
//    int     first;             /* first element to read */
//
//} fifo_struct;
//
//fifo_struct arrFifos[MAXFIFO];
//static int numFifos = 0; /* keeps track of the number of fifos */

//FIFO OS_InitFiFo()
//{
//    /* gets next available fifo */
//    if ( numFifos < MAXFIFO )
//    {
//        arrFifos[numFifos].fillCount = 0;
//        arrFifos[numFifos].next = 0;
//        arrFifos[numFifos].first = 0;
//        ++numFifos;
//        return numFifos;
//    }
//    else
//    {
//        /* ran out of fifos */
//        return INVALIDFIFO;
//    }
//}
//
//void OS_Write(FIFO f, int val)
//{
//    int idx = f - 1;
//    fifo_struct *curFifo = &arrFifos[idx];
//
//    /* the buffer still has space to write
//       if it is full writes are ignored */
//    if ( curFifo->fillCount < FIFOSIZE )
//    {
//        curFifo->buffer[curFifo->next] = val;
//        curFifo->next = (curFifo->next + 1) % FIFOSIZE;
//    }
//
//    /* increment fillcount if not full */
//    curFifo->fillCount = (curFifo->fillCount == FIFOSIZE) ? FIFOSIZE : ++curFifo->fillCount;
//}
//
//BOOL OS_Read(FIFO f, int *val)
//{
//    int idx = f - 1;
//    fifo_struct *curFifo = &arrFifos[idx];
//
//    /* the buffer is empty */
//    if ( curFifo->fillCount <= 0 )
//    {
//        return FALSE;
//    }
//    /* there are still elements in the buffer */
//    else
//    {
//        *val = curFifo->buffer[curFifo->first];
//        curFifo->first = (curFifo->first + 1) % FIFOSIZE;
//        curFifo->fillCount--;
//        return TRUE;
//    }
//}
