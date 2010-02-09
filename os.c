/*******************************************************************
*
*    DESCRIPTION: This is a kernel for a robot.
*    AUTHOR: Eduardo Gutarra Velez.
*    DATE: 2/19/2010
*
*******************************************************************/

#include "os.h"
#include "interrupts.h"
#include "fifo.h"
#include <stdlib.h>

typedef enum 
{
    NEW, RUNNING, WAITING, READY, TERMINATED
} 
States;

typedef struct pcb_struct 
{
    PID         pid;
    int         level;
    States      state;
    int         argument; /* argument */
    char        *sp; /* stack pointer */
    int         pc; /* program counter */
    FIFO        fifo; /* message queue */
    int         frequency;
    int         name;
} ProcCtrlBlock;

typedef struct ProcQueue
{
    ProcCtrlBlock*  procRefs[MAXPROCESS];  /* message circular buffer */
    int             fillCount;         /* keeps track of the number of items */          
    int             next;              /* next spot to write in */
    int             first;             /* first element to read */

} ProcQueue;


ProcQueue devProcs;
ProcQueue spoProcs;

ProcCtrlBlock  arrProcs[MAXPROCESS];
ProcCtrlBlock  idleProc;
ProcCtrlBlock* currProc;

int PPP[MAXPROCESS];           
int PPPMax[MAXPROCESS];
int PPPLen;

static int schedIdx;

char acWorkspaces[MAXPROCESS*WORKSPACE];

void idle()
{
//  printf("idle %d \n", -1);
}


void idle0()
{
//  printf("idle %d \n", 0);
}


void idle1()
{
//  {
//  printf("idle %d \n", 1);
//  }
}

void Enqueue(ProcQueue* prq, ProcCtrlBlock* p);
BOOL Dequeue(ProcQueue *prq, ProcCtrlBlock** p);
void InitQueues();

void OS_Yield(void)
{
    /* trigger a context switch */
}

int  OS_GetParam(void)
{
    return currProc->argument;
}

/* Semaphore primitives */
void OS_InitSem(int s, int n)
{
}


void OS_Wait(int s)
{
}


void OS_Signal(int s)
{
}

/* OS Initialization */
void OS_Init(void)
{
    int i;
    /* Initialize PCBs */
    for ( i = 0; i < MAXPROCESS; ++i )
    {
        arrProcs[i].pid          = i + 1;
        arrProcs[i].level        = -1;
        arrProcs[i].state        = TERMINATED;
        arrProcs[i].argument     = 0;
        arrProcs[i].pc           = 0;
    }

    /* Initialize idle process */
    idleProc.level     = PERIODIC;
    idleProc.state     = READY;
    idleProc.argument  = 0;
    /* TODO: does it need a workspace?*/
    idleProc.pc        = (int) &idle;

    InitQueues();
    /* TODO: determine quantum I can hardcore that */
    /* TODO: use the same hw setup interrupt ot increment a timer */
    /* TODO: enable interrupts? */
}
void OS_Start(void)
{
    /**
     * If no task is running, and all tasks are not in the ready 
     * state, the idle task executes 
     * */ 

    idle();

}

void context_switch (void)
{
    OS_DI(); /* disable all interrupts */

    OS_EI(); /* enable all interrupts */  
}
void OS_Abort()
{
    //EXIT();
}

PID  OS_Create(void (*f)(void), int arg, unsigned int level, unsigned int n)
{
    int idx, pid = INVALIDPID;

    if ( level == PERIODIC ) /* Checking if a name n is already taken. */
    {
        for ( idx = 0; idx < MAXPROCESS; ++idx )
        {
            if ( arrProcs[idx].name == n )
            {
                return INVALIDPID;
            }
        }
    }
    for ( idx = 0; idx < MAXPROCESS; ++idx ) /* Searching an available block */
    {
        pid = arrProcs[idx].pid;
        if ( arrProcs[idx].state == TERMINATED )
        {
            arrProcs[idx].level       = level;
            arrProcs[idx].state       = NEW;
            arrProcs[idx].argument    = arg;
            arrProcs[idx].sp          = acWorkspaces + (WORKSPACE*idx);
            arrProcs[idx].pc          = (int) f;

            switch ( level )
            {
            case SPORADIC:
                Enqueue(&spoProcs, &arrProcs[idx]);
                break;
            case DEVICE:
                Enqueue(&devProcs, &arrProcs[idx]);
                arrProcs[idx].frequency = n;
                break;
            case PERIODIC:
                arrProcs[idx].name = n;
                break;
            default:
                OS_Abort();
            }
            return pid;
        }
    }
    return INVALIDPID;
}

void OS_Terminate(void)
{
    currProc->state = TERMINATED;
    currProc->name = -1;
    currProc->frequency = 0;

    if ( currProc->level == SPORADIC )
    {
        /* TODO: remove from sporadic queue */
    }
    else if ( currProc->level == DEVICE )
    {
        /* TODO: remove from device queue */
    }
    /* TODO: need a context switch */
}

void OS_Schedule(void)
{

//  for ( int i = 0; i < PPPLen; ++i )
//  {
//      set_interrupt(PPPMax[i]);
//      context_switch(PPP[i]);
//  }

    /* scheduler is the */
    int i;
    for ( i = 0; i < MAXPROCESS; i++ )
    {
        if ( arrProcs[i].name == PPP[schedIdx] )
        {
            currProc = &arrProcs[i];
            break;
        }
    }

    if ( schedIdx == PPPLen - 1 )
    {
        schedIdx = 0;
    }
    else
    {
        ++schedIdx;
    }
}

void Enqueue(ProcQueue* prq, ProcCtrlBlock* p)
{
    /* the queue still has space to write
       if it is full writes are ignored */
    if ( prq->fillCount < MAXPROCESS )
    {
        prq->procRefs[prq->next] = p;
        prq->next = (prq->next + 1) % MAXPROCESS;
    }

    /* increment fillcount if not full */
    prq->fillCount = (prq->fillCount == MAXPROCESS) ? MAXPROCESS : ++prq->fillCount;
}

BOOL Dequeue(ProcQueue *prq, ProcCtrlBlock** p)
{
    /* The queue is empty */
    if ( prq->fillCount <= 0 )
    {
        return FALSE;
    }
    /* there are still elements in the queue */
    else
    {
        *p = prq->procRefs[prq->first];
        prq->first = (prq->first + 1) % MAXPROCESS;
        prq->fillCount--;
        return TRUE;
    }
}

void InitQueues()
{
    devProcs.fillCount = 0;
    devProcs.next = 0;
    devProcs.first = 0;

    spoProcs.fillCount = 0;  
    spoProcs.next = 0;       
    spoProcs.first = 0;      
}
 
 
 
FIFO OS_InitFiFo()
{
    /* gets next available fifo */
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
        /* ran out of fifos */
        return INVALIDFIFO;
    }
}

void OS_Write(FIFO f, int val)
{
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
}

BOOL OS_Read(FIFO f, int *val)
{
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
}
