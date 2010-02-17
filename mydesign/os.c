/*******************************************************************
*
*    DESCRIPTION: This is a kernel for the motorolla robot with
*    processor MC11.
*    AUTHOR: Eduardo Gutarra Velez.
*    DATE: 2/19/2010
*
*******************************************************************/

#include "os.h"
#include "interrupts.h"
#include "ports.h"
#include "fifo.h"
#include "process.h"
//#include "semaphores.h"

ProcQueue devProcs;
ProcQueue spoProcs;

int PPP[MAXPROCESS];           
int PPPMax[MAXPROCESS];
int PPPLen;

// plus one workspace for the idle process.
char acWorkspaces[MAXPROCESS*WORKSPACE]; 
char idleWorkspace[WORKSPACE];
//Semaphore semArr[MAXSEM];

ProcCtrlBlock  arrProcs[MAXPROCESS];
ProcCtrlBlock  idleProc;
ProcCtrlBlock* currProc;

void timeToPreempt(unsigned int);

void OS_Yield(void)
{
    SWI();
}

int  OS_GetParam(void)
{
    return currProc->argument;
}

//void OS_InitSem(int s, int n)
//{
//    semArr[s].count = n;
//}
//
//void OS_Wait(int s)
//{
//    if ( semArr[s].count > 0 )
//    {
//        semArr[s].count--;
//    }
//    else
//    {
//        Enqueue(semArr[s].procQueue, currProc);
//        /*block ()*/
//    }
//}
//
//
//void OS_Signal(int s)
//{
//    semArr[s].count++;
//    if(semArr[s].procQueue->fillCount > 0)
//    {
//        Dequeue(semArr[s].procQueue, &currProc);
//        /*wakeup(P)*/
//    }
//}

/* OS Initialization */
void OS_Init(void)
{
    int i;

    unsigned char*  stackPointer;
    unsigned int*   programCounter;

    /* Initialize PCBs */
    for ( i = 0; i < MAXPROCESS; ++i )
    {
        arrProcs[i].pid          = i + 1;
        arrProcs[i].level        = -1;
        arrProcs[i].state        = TERMINATED;
        arrProcs[i].argument     = 0;
        arrProcs[i].pc           = 0;
    }


    InitQueues();

    /* initialize the stack for the idle process*/
//  stackPointer = currProc->sp;
//  programCounter = (unsigned int*) stackPointer - 1;
//  *programCounter = (unsigned int) currProc->pc;
//  currProc->sp = (unsigned char*) (stackPointer - 18);

    SWIV  = context_switch;
    TOC4V = context_switch;
}
void OS_Start(void)
{

    /* Initialize idle process Control Block */
    idleProc.pid = IDLE;
    idleProc.state = RUNNING;

    currProc = &idleProc;

    B_SET(_io_ports[TMSK1], 4);

    _io_ports[TOC4] = _io_ports[TCNT] + 20000;
    
    while ( TRUE )
    {
        serial_print("IIIIII\n");
    }
//  idle();
//  context_switch();
}

void OS_Abort()
{
    SWI();
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
            arrProcs[idx].sp          = acWorkspaces + (WORKSPACE*pid);
            arrProcs[idx].pc          = f;

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

void SWI()
{
    asm volatile ("swi");
}

void RTI()
{
    asm volatile ("rti");
}

void OS_Terminate(void)
{
    OS_DI();
    currProc->state = TERMINATED;
    currProc->name = -1;
    currProc->frequency = 0;

    ProcCtrlBlock *p0; /* choose next process to run */
    BOOL found = FALSE;
    if ( currProc->level == SPORADIC )
    {
        while ( found == FALSE  && Dequeue(&spoProcs, &p0) == TRUE )
        {
            if ( p0 == currProc )
            {
                /* if found dequeuing removes it */
                found = TRUE;
            }
            else
            {
                /* if not found put it back in the queue */
                Enqueue(&spoProcs, p0);
            }
        }
    }
    else if ( currProc->level == DEVICE )
    {
        while ( found == FALSE  && Dequeue(&devProcs, &p0) == TRUE )
        {
            if ( p0 == currProc )
            {
                /* if found dequeuing removes it */
                found = TRUE;
            }
            else
            {
                /* if not found put it back in the queue */
                Enqueue(&devProcs, p0);
            }
        }
    }
    SWI();
}

void Schedule(void)
{
    static unsigned int SchedIdx = 0;
    ProcCtrlBlock *p0; /* choose next process to run */
    BOOL found = FALSE; 
    unsigned int idx = 0;
    unsigned int timeInMs;
    /* choose how long to run it for */

    /* TODO: if device process is due schedule it */
    if ( PPP[SchedIdx] == IDLE )
    {
        if ( Dequeue(&spoProcs, &p0) == TRUE )
        {
            currProc = p0;
            Enqueue(&spoProcs, p0);
        }
        else
        {
            /* if there are no sporadic processes then idle runs*/
            currProc = &idleProc;
        }
    }
    else
    {
        /* Searching the corresponding block to the name of the process */
        for ( idx = 0; idx < MAXPROCESS; ++idx )
        {
            if ( arrProcs[idx].name == PPP[SchedIdx] )
            {
                currProc = &arrProcs[idx];
                break;
            }
        }
    }    
    
    timeInMs = PPPMax[SchedIdx];
    SchedIdx = (SchedIdx + 1) % PPPLen;
//  timeToPreempt(timeInMs);

    _io_ports[TOC4] = _io_ports[TCNT] + timeInMs * TICKS_IN_MS;
    /* Set the bomb */
    B_SET(_io_ports[TMSK1], 4);
}

void timeToPreempt(unsigned int timeInMs)
{
    _io_ports[TOC4] = _io_ports[TCNT] + timeInMs * 2000;
    /* Set the bomb */
    B_SET(_io_ports[TMSK1], 4);
}

void setProcessStack()
{
    unsigned char*  stackPointer;
    unsigned int*   programCounter;

    stackPointer = currProc->sp;

//  stackPointer = ((currProc->pc) & 0x00FF)
//  (stackPointer - 1) = ((currProc->pc) & 0xFF00) >> 8;
    programCounter = (unsigned int*) stackPointer - 1;
    *programCounter = (unsigned int) currProc->pc;
    currProc->sp = (unsigned char*) (stackPointer - 18);
}
__attribute__ ((interrupt)) void context_switch (void)
{
    B_SET(_io_ports[TFLG1], 4);
    B_UNSET(_io_ports[TMSK1], 4);

    if ( currProc->state == RUNNING )
    {
        currProc->state = READY;
    }

    asm volatile ("sts %0" : "=m" (currProc->sp) : : "memory" ); 

    Schedule(); /* selects the next process and updates the */

    if ( currProc->state == NEW )
    {
        setProcessStack();
        currProc->state = READY;
    }

    asm volatile ("lds %0" : : "m" (currProc->sp) : "memory");

    currProc->state = RUNNING;
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
    /* device processes queue */
    devProcs.fillCount = 0;
    devProcs.next = 0;
    devProcs.first = 0;

    /* sporadic processes queue */
    spoProcs.fillCount = 0;  
    spoProcs.next = 0;       
    spoProcs.first = 0;      
}
