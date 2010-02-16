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
    void        (*pc)(void); // function pointer
    int         frequency;
    int         name;
} ProcCtrlBlock;

typedef struct queue_struct
{
    ProcCtrlBlock*  procRefs[MAXPROCESS];  /* message circular buffer */
    int             fillCount;         /* keeps track of the number of items */          
    int             next;              /* next spot to write in */
    int             first;             /* first element to read */

} ProcQueue;

typedef struct kernel_struct
{
    char* kernelSP;
} KernelCtrlBlock;

KernelCtrlBlock kernSp;

ProcQueue devProcs;
ProcQueue spoProcs;

ProcCtrlBlock  arrProcs[MAXPROCESS];
ProcCtrlBlock  idleProc;
ProcCtrlBlock* currProc;

int PPP[MAXPROCESS];           
int PPPMax[MAXPROCESS];
int PPPLen;

char acWorkspaces[MAXPROCESS*WORKSPACE];

void SWI(void);
void Enqueue(ProcQueue* prq, ProcCtrlBlock* p);
BOOL Dequeue(ProcQueue *prq, ProcCtrlBlock** p);
void InitQueues();
void Schedule(void);
void __attribute__ ((interrupt)) contextSwitch (void);

void idle(void)
{
    while ( TRUE )
    {
    }
}


void OS_Yield(void)
{
    SWI();
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
    idleProc.pc        = idle;

    InitQueues();
    /* TODO: determine quantum I can hardcore that */
    /* TODO: use the same hw setup interrupt ot increment a timer */
    /* TODO: enable interrupts? */
    currProc = &idleProc;

//  SWIV  = contextSwitch;
	TOC4V = contextSwitch;
}
void OS_Start(void)
{
    B_SET(_io_ports[TMSK1], 4);

    _io_ports[TOC4] = _io_ports[TCNT] + 20000;

    while ( TRUE )
    {
        serial_print("can't get out of here!\n");
    }
}

void OS_Abort()
{
    //EXIT();
}

PID  OS_Create(void (*f)(void), int arg, unsigned int level, unsigned int n)
{
    int idx, pid = INVALIDPID;
    unsigned char*  stackBase;
    unsigned char*  ccr;
    unsigned int*   pc;

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
//          arrProcs[idx].sp          = acWorkspaces + (WORKSPACE*idx);
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

            stackBase = acWorkspaces + (WORKSPACE*idx);
            pc = (unsigned int*) stackBase - 1;
            ccr = (unsigned char*) stackBase - 3;
            *pc = (unsigned int) f;
            *ccr = 0x00;
        
            arrProcs[idx].sp = (unsigned char*) (stackBase - 18);

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
    static int SchedIdx;
    ProcCtrlBlock *p0; /* choose next process to run */
    BOOL found = FALSE; 
    unsigned int timeInMs, idx = 0;
    /* choose how long to run it for */

    /* if device process is due schedule it */
    if ( PPP[SchedIdx] == IDLE )
    {
        if ( Dequeue(&spoProcs, &p0) == TRUE )
        {
            currProc = p0;
            Enqueue(&spoProcs, p0);
        }
        else
        {
            currProc = &idleProc;
        }
    }
    else
    {
        for ( idx = 0; idx < MAXPROCESS; ++idx ) /* Searching an available block */
        {
            if (arrProcs[idx].name == PPP[SchedIdx])
            {
                currProc = &arrProcs[idx];
                break;
            }
        }
    }    

    timeInMs = PPPMax[SchedIdx];
    SchedIdx = (SchedIdx == PPPLen - 1) ? 0 : SchedIdx + 1;

    _io_ports[TOC4] = _io_ports[TCNT] + timeInMs * 2000;

    /* Set the bomb */
    B_SET(_io_ports[TMSK1], 4);
}

void __attribute__ ((interrupt)) contextSwitch (void)
{
    B_SET(_io_ports[TFLG1], 4);
    B_UNSET(_io_ports[TMSK1], 4);

    if ( currProc->state == RUNNING )
    {
        currProc->state = READY;
    }

    storeSP(currProc->sp);

    Schedule(); /* Selects the next process and updates the currentProcess pointer */

    loadSP(currProc->sp);

    /* check message queue */

//    if ( currProc->state == NEW )
//    {
//        asm volatile ("lds %0" : : "m" (currProc->sp) : "memory");
//        currProc->state = READY;
//        currProc->pc(); /* call the function for the first time */
//        OS_Terminate();
//    }
//    else if ( currProc->state == READY  && currProc->state != TERMINATED)
//    {
//        asm volatile ("lds %0" : : "m" (currProc->sp) : "memory");
////      asm volatile ("rti");  /* returning where that function was before */
//        RTI();
////      currProc->pc();
////      OS_Yield();
//    }
//    else
//    {
//        SWI();
////      asm volatile ("lds %0" : : "m" (kernSp.kernelSP) : "memory");
////      asm volatile ("rti");
//    }

    /* Set the CPU's stack pointer so RTI can unwind the new process's stack */
//  asm("lds %0" : : "m" (currProc->sp));

//  RTI();

//  /* Check the message queue */
//  if(currProc->fifo != INVALIDFIFO)
//  {
//  	/* Note: signals are undefined at this point */
//  }
//
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

FIFO OS_InitFiFo()
{
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
