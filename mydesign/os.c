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
#include "semaphores.h"

ProcQueue devProcs;
ProcQueue spoProcs;

int PPP[MAXPROCESS];           
int PPPMax[MAXPROCESS];
int PPPLen;

char acWorkspaces[MAXPROCESS*WORKSPACE]; 
char idleWorkspace[WORKSPACE/4];
Semaphore semArr[MAXSEM];

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

/* idle process */
void idle ( void )
{
    while ( TRUE )
    {
        serial_print("IIIIII\n");
    }
}

/* OS Initialization */
void OS_Init(void)
{
    B_SET(_io_ports[TMSK2], 0); // slows clock down for testing
    B_SET(_io_ports[TMSK2], 1);

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

    /* Initialize idle process Control Block */
    idleProc.pid = IDLE;
    idleProc.state = NEW;
    idleProc.pc = &idle;
    idleProc.sp = (unsigned int*) (idleWorkspace + WORKSPACE/4);

    InitQueues();

    SWIV  = context_switch;
    TOC4V = context_switch;
}

void OS_InitSem(int s, int n)
{
    semArr[s].value = n;
}

void OS_Wait(int s)
{
    OS_DI(); /* disable interrupts to perform as atomic operation */

    if ( semArr[s].value > 0 )
    {
        semArr[s].value--;
        OS_EI();
    }
    else
    {
        semArr[s].procQueue[semArr[s].procCount++] = currProc;

        if ( currProc->level == SPORADIC )
        {
            currProc->state = WAITING;
        }

        OS_Yield();
    }
}

void OS_Signal(int s)
{
    int i; /* iterator */
    ProcCtrlBlock *p0; 

    OS_DI(); /* disable interrupts to perform as atomic operation */

    semArr[s].value++;

    
    if ( semArr[s].procCount > 0 )
    {
        p0 = semArr[s].procQueue[0];
        p0->state = READY;
//      Enqueue(&spoProcs, p0);
        
		for(i = 1; i < semArr[s].procCount; i++)
		{
			semArr[s].procQueue[i - 1] = semArr[s].procQueue[i];
		}

        currProc->state = READY;
        semArr[s].procCount--;
    }
//  OS_EI();
    OS_Yield();
}

void OS_Start(void)
{
    currProc = 0;
    context_switch();
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
            arrProcs[idx].sp          = (unsigned int*) (acWorkspaces + (WORKSPACE*pid));
            arrProcs[idx].pc          = f;

            switch ( level )
            {
            case SPORADIC:
                Enqueue(&spoProcs, &arrProcs[idx]);
                break;
            case DEVICE:
                Enqueue(&devProcs, &arrProcs[idx]);
                arrProcs[idx].frequency = n;
                arrProcs[idx].countDown = n;
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
    /* TODO: handle case when process is periodic*/
    SWI();
}

BOOL NextDevProc(unsigned int timeInMs)
{
    unsigned int count = 0;
    ProcCtrlBlock *p0;

    while ( count < devProcs.fillCount )
    {
        if ( Dequeue(&devProcs, &p0) == TRUE )
        {
            /* device process is due to run */
            if ( p0->countDown <= 0 )
            {
                currProc = p0;
                p0->countDown = p0->frequency;
                Enqueue(&devProcs, p0);
                return TRUE;
            }
            else
            {
                p0->countDown -= timeInMs;
            }
            Enqueue(&devProcs, p0);
            count += 1;
        }
    }
    return FALSE;
}

void Schedule(void)
{
    static unsigned int SchedIdx = 0;
    ProcCtrlBlock *p0; /* choose next process to run */
    unsigned int idx = 0;
    unsigned int timeInMs;

    /* choose how long to run it for */
    timeInMs = PPPMax[SchedIdx];

    if ( !NextDevProc(timeInMs) )
    {
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

        SchedIdx = (SchedIdx + 1) % PPPLen;
        if ( !(currProc->state == READY || currProc->state == NEW) )
        {
            OS_Yield();
        }
        timeToPreempt(timeInMs);
    }

    if ( !(currProc->state == READY || currProc->state == NEW) )
    {
        OS_Yield();
    }
}

void timeToPreempt(unsigned int timeInMs)
{
    _io_ports[TOC4] = _io_ports[TCNT] + timeInMs * TICKS_IN_MS;
    /* Set the bomb */
    B_SET(_io_ports[TMSK1], 4);
    B_SET(_io_ports[TFLG1], 4);
}

void setProcessStack()
{
    unsigned int*  stackPointer;
//  unsigned char *sp, t;

    /* store lo byte */
//  t = (unsigned) pf & 0xff;
//  sp = (unsigned char *) t;

    /* store hi byte */
//  t = (unsigned) pf >> 8;
//  sp = (unsigned char *) t;

    stackPointer = currProc->sp;
    *(unsigned int*)(stackPointer - 1) = (unsigned int) currProc->pc;
    currProc->sp = (unsigned int*) (stackPointer - 9);
}

__attribute__ ((interrupt)) void context_switch (void)
{
    OS_DI();
    B_UNSET(_io_ports[TFLG1], 4);
    B_UNSET(_io_ports[TMSK1], 4);

    if ( currProc != 0 )
    {
        asm volatile ("sts %0" : "=m" (currProc->sp) : : "memory" ); 
    }

    Schedule(); /* selects the next process due to run */

    if ( currProc->state == NEW )
    {
        setProcessStack();
        currProc->state = READY;
    }
    OS_EI();
    asm volatile ("lds %0" : : "m" (currProc->sp) : "memory");

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
