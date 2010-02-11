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
#include "fifo.h"
#include "lcd.h"
#include <stdlib.h>


///* source: http://www.catonmat.net/blog/bit-hacks-header-file/ */
///* set n-th bit in x */
//#define B_SET(x, n)      ((x) |= (1<<(n)))
///* unset n-th bit in x */
//#define B_UNSET(x, n)    ((x) &= ~(1<<(n)))
///* toggle n-th bit in x */
//#define B_TOGGLE(x, n)   ((x) ^= (1<<(n)))
//
//
//#define TMSK1   (*(volatile int *)(VECTOR_BASE + 0x22))
//#define TFLG1   (*(volatile int *)(VECTOR_BASE + 0x23))
//#define TMSK2   (*(volatile int *)(VECTOR_BASE + 0x24))

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

typedef struct queue_struct
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

char acWorkspaces[MAXPROCESS*WORKSPACE];


void sys_print_lcd(char* text) {
	unsigned char i = 0;
        unsigned int k;
	
	*(volatile unsigned char *)(0xfe) = 0;
	*(volatile unsigned char *)(0xff) = 3;
//  __asm__ __volatile__ ("jsr 32");
	
	for (k = 1; k != 0; k++);
	
	*(volatile unsigned char *)(0xfe) = 2;
	while (*text != 0 && i < MAX_CHAR_COUNT) {
		*(volatile unsigned char *)(0xff) = *text;
//  	__asm__ __volatile__ ("jsr 32");
		text++;
		i++;
	}
	
}

static void _sys_send_command_lcd(void) {
///    __asm__ __volatile__ ("
///        ldx     #4096
///        bclr    0,X #16
///        bclr    60,X    #32
///        bclr    0,X     #16
///        ldaa    #255
///        staa    7,X
///ldaa	254
///        staa    4,X
///        ldab    255
///        stab    3,X
///        bset    0,X     #16
///        bclr    0,X     #16
///        clr     7,X
///wait:	ldaa	#1
///        staa    4,X
///        bset    0,X     #16
///        ldab    3,X
///        bclr    0,X     #16
///        andb    #128
///        beq     wait
///
///Done:	bset	60,X	#32 ");
}

void _sys_init_lcd(void) {
	void* sys_print_loc =  &_sys_send_command_lcd;
	void* internal_mem = (void *)0x0020;
	
	for (; internal_mem < (void *) 0x00ff; internal_mem++, sys_print_loc++)
		*(unsigned char *)(internal_mem) = *(unsigned char *)(sys_print_loc);

	*(volatile unsigned char *)(0xfe) = 0;
	*(volatile unsigned char *)(0xff) = 15;

	__asm__ __volatile__ ("jsr 32");
}


void idle()
{
//  printf("idle %d \n", -1);
}


void spo1()
{
//  printf("I'm spo %d \n", 1);
}


void spo2()
{
//  printf("I'm spo %d \n", 2);
}

void Enqueue(ProcQueue* prq, ProcCtrlBlock* p);
BOOL Dequeue(ProcQueue *prq, ProcCtrlBlock** p);
void InitQueues();
void Schedule(void);

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

__attribute__ ((interrupt))
void context_switch (void)
{
    B_SET(TFLG1, 4);
    B_UNSET(TMSK1, 4); /* disable OC4 interrupt*/

    if ( currProc->level == SPORADIC )
    {
        currProc->state = READY;
    }
    storeSP(currProc->sp);

    Schedule();

    loadSP(currProc->sp);



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

    TOC4V = &context_switch;
    SWIV  = &context_switch;
}
void OS_Start(void)
{
    /**
     * If no task is running, and all tasks are not in the ready 
     * state, the idle task executes 
     * */ 
    B_SET(TMSK1, 4); /* enable oc4 interrupt */
    B_SET(TMSK2, 7 ); /* enable toi interrupt */
//  asm("swi"); /* trigger sw interrupt*/

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

    ProcCtrlBlock *p0;
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
                /* if not found put it back in the queue*/
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
                /* if not found put it back in the queue*/
                Enqueue(&devProcs, p0);
            }
        }
    }
    /* TODO: need a context switch */
}

void Schedule(void)
{

    ProcCtrlBlock *p0;
    BOOL found = FALSE;
    if ( Dequeue(&spoProcs, &p0) == TRUE )
    {
        currProc = p0;
    }
    currProc->state = RUNNING;

    /* for the other ugly process */
//  int i;
//  for ( i = 0; i < MAXPROCESS; i++ )
//  {
//      if ( arrProcs[i].name == PPP[schedIdx] )
//      {
//          currProc = &arrProcs[i];
//          break;
//      }
//  }
//
//  if ( schedIdx == PPPLen - 1 )
//  {
//      schedIdx = 0;
//  }
//  else
//  {
//      ++schedIdx;
//  }
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
