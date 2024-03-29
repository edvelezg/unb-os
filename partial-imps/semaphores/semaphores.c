#include "os.h"
#include "fifo.h"
#include "ports.h"
#include "interrupts.h"
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


typedef struct semaphore_struct
{
	ProcQueue	procQueue[MAXPROCESS];
	int 		count;

} Semaphore;

Semaphore semArr[MAXSEM];

ProcQueue devProcs;
ProcQueue spoProcs;

ProcCtrlBlock  arrProcs[MAXPROCESS];
ProcCtrlBlock  idleProc;
ProcCtrlBlock* currProc;

int PPP[MAXPROCESS];           
int PPPMax[MAXPROCESS];
int PPPLen;
volatile static int scheduleIdx = 0;

char acWorkspaces[MAXPROCESS*WORKSPACE];

void SWI(void);
void Enqueue(ProcQueue* prq, ProcCtrlBlock* p);
BOOL Dequeue(ProcQueue *prq, ProcCtrlBlock** p);
void InitQueues();
void Schedule(void);
//void context_switch_to_kernel (void);
//void context_switch_to_process (void);
//void __attribute__ ((interrupt)) contextSwitch (void);

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
	semArr[s].count = n;
}


void OS_Wait(int s)
{	
//  OS_DI();
	
    if ( semArr[s].count > 0 )
    {
        semArr[s].count--;	
    }
	else
	{
        Enqueue(semArr[s].procQueue, currProc);
        /*block ()*/
	}
////    else
////    {
////        /* Add the process to the semaphore's wait queue */
////        semaphoreList[s].queue[semaphoreList[s].queueLen++] = currentProcess;
////
////        /* Update the sporadic queue if necessary */
////        if(currentProcess->policy == SPORADIC)
////        {
////            for(i = 1; i < sporadicQueueLen - 1; i++) /* Queue guaranteed to have length > 0 */
////            {
////                sporadicQueue[i - 1] = sporadicQueue[i];
////            }
////
////            sporadicQueueLen--;
////        }
////
////        currentProcess->state = WAITING;
////
////        OS_EI();
//////  	OS_Yield();
////    }
//
    printf("val: %d queue: %d \n", semArr[s].count, semArr[s].procQueue->fillCount);
//
}


void OS_Signal(int s)
{
    semArr[s].count++;	
	if(semArr[s].procQueue->fillCount > 0)
	{
        Dequeue(semArr[s].procQueue, &currProc);
        /*wakeup(P)*/
	}
////    else
////    {
////        /* Add the process to the semaphore's wait queue */
////        semaphoreList[s].queue[semaphoreList[s].queueLen++] = currentProcess;
////
////        /* Update the sporadic queue if necessary */
////        if(currentProcess->policy == SPORADIC)
////        {
////            for(i = 1; i < sporadicQueueLen - 1; i++) /* Queue guaranteed to have length > 0 */
////            {
////                sporadicQueue[i - 1] = sporadicQueue[i];
////            }
////
////            sporadicQueueLen--;
////        }
////
////        currentProcess->state = WAITING;
////
////        OS_EI();
//////  	OS_Yield();
////    }
//
    printf("val: %d queue: %d \n", semArr[s].count, semArr[s].procQueue->fillCount);
//
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
}
void OS_Start(void)
{
    /**
     * If no task is running, and all tasks are not in the ready 
     * state, the idle task executes 
     * */ 
//  while ( 1 )
//  {
//      Schedule();
//  }

    SWI();
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
//  asm volatile ("swi");
}

void RTI()
{
//  asm volatile ("rti");
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
    ProcCtrlBlock *p0; /* choose next process to run */
    BOOL found = FALSE; 
    int idx = 0;
    /* choose how long to run it for */

    /* if device process is due schedule it */
    if ( PPP[scheduleIdx] == IDLE )
    {
        if ( Dequeue(&spoProcs, &p0) == TRUE )
        {
            currProc = p0;
            Enqueue(&spoProcs, p0);
//            if ( currProc->state == NEW )
//            {
//                currProc->state = READY;
//          currProc->pc();
//            }
//          else if ( currProc->state == READY )
//          {
//
//          }
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
            if (arrProcs[idx].name == PPP[scheduleIdx])
            {
                currProc = &arrProcs[idx];
                break;
            }
        }
//      currProc->pc();
    }    
    scheduleIdx = (scheduleIdx + 1) % PPPLen;
    /* run it */
//  if (currProc != 0)
//  {
//  	SWI(); /* triggers context_switch */
//  }
}

//void __attribute__ ((interrupt)) contextSwitch (void)
//{
//    if ( currProc->state == RUNNING )
//    {
//        currProc->state = READY;
//    }
//
//    /* Save the CPU's stack pointer in the current process's control block */
////  asm("sts %0" : : "m" (currProc->sp));
//
//    Schedule(); /* Selects the next process and updates the currentProcess pointer */
//
//    if ( currProc->state == NEW )
//    {
////      asm volatile ("lds %0" : : "m" (currProc->sp) : "memory");
//        currProc->state = READY;
//        currProc->pc(); /* call the function for the first time */
//        OS_Terminate();
//    }
//    else if ( currProc->state == READY  && currProc->state != TERMINATED)
//    {
////      asm volatile ("lds %0" : : "m" (currProc->sp) : "memory");
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
//
//    /* Set the CPU's stack pointer so RTI can unwind the new process's stack */
////  asm("lds %0" : : "m" (currProc->sp));
//
////  RTI();
//
////  /* Check the message queue */
////  if(currProc->fifo != INVALIDFIFO)
////  {
////  	/* Note: signals are undefined at this point */
////  }
////
////  currentProcess->state = RUNNING;
//}


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

int main (int argc, char *argv[])
{
    OS_InitSem(5, 5);
    OS_Wait(5);
    OS_Wait(5);
    OS_Wait(5);
    OS_Wait(5);
    OS_Wait(5);
    OS_Signal(5);
    OS_Signal(5);
    OS_Signal(5);
    OS_Signal(5);
    OS_Signal(5);
    OS_Wait(5);
    OS_Wait(5);
    OS_Wait(5);
    OS_Signal(5);
    OS_Signal(5);
    OS_Signal(5);
    OS_Signal(5);
    OS_Signal(5);
    OS_Signal(5);
    OS_Signal(5);
    OS_Signal(5);
    OS_Signal(5);
    OS_Signal(5);
    return 0;
}
