#include <stdlib.h>
#include "os.h"
/**
 * Notes: 
 * Stack pointer address: 512*i - x 
 */

//char Memory[MAXPROCESS*WORKSPACE];
//What are the different process states?
//.. new, running, waiting, ready,
//terminated.

// WHAT IS THE DIAGRAM LIKE FOR THESE STATES?
// Don't I need to know what function I"m pointing to with the PCB

enum states
{
    NEW, RUNNING, WAITING, READY, TERMINATED
};

struct pcb 
{
    PID         pid;
    int         level;
    states      state;
    int         arg; /* argument */
    char        *pcSp; /* stack pointer */
    int         pc; /* program counter */
    FIFO        fifo; /* message queue */
    int         frequency;
    BOOL        available; /* whether the PCB is taken by a process or not */
};

struct pcb atProcesses[MAXPROCESS];
struct pcb  tIdle;
struct pcb* ptCurrent;

int PPP[MAXPROCESS];           
int PPPMax[MAXPROCESS];
int PPPLen;      

char acWorkspaces[MAXPROCESS*WORKSPACE];


void idle()
{
    while ( 1 );
}

void OS_Terminate(void)
{
}

void OS_Yield(void)
{
}

int  OS_GetParam(void)
{
    return ptCurrent->arg;
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

/** 
 * The function main() will be called first by crt11.s.  Before
 * any calls can be placed to the OS, main() must call OS_Init() to * 
 * initialize the OS. * main() can then create processes and * 
 * initialize the PPP[] and PPPMax[] * arrays. To boot the OS, * 
 * main() must call OS_Start() which never returns. * Assumption: * 
 * OS_Init() is called exactly once at boot time. * * 
 */

/* OS Initialization */
void OS_Init(void)
{
    /* Initialize PCBs */
    for ( int i = 0; i < MAXPROCESS; ++i )
    {
        atProcesses[i].level           = PERIODIC;
        atProcesses[i].state           = NEW;
        atProcesses[i].arg             = 0;
        atProcesses[i].pcSp            = acWorkspaces + (WORKSPACE*i);
        atProcesses[i].pc              = 0;
        atProcesses[i].fifo            = INVALIDFIFO;
        atProcesses[i].available       = TRUE;
    }

    /**
     * What if I have 17*WORKSPACE process instead, in which I count
     * process 0 as the INVALIDPID -- idle process -- and it 
     * represents the first block of all blocks.. I would then count 
     * from 1 to MAXPROCESS 
     *  
     */
    tIdle.level           = PERIODIC;
    tIdle.state           = READY;
    tIdle.arg             = 0;
    tIdle.pcSp            = acWorkspaces + (WORKSPACE*0); /* dOES IT NEED A WORKSPACE?*/
    tIdle.pc              = (int) &idle;
    tIdle.fifo            = INVALIDFIFO;
    tIdle.available       = FALSE;

    //  determine quantum .. i can hardcode this value because I am using the same HW
    //  setup an interrupt to increment a timer
    //  enable interrupts
}
void OS_Start(void)
{
    /**
     * If no task is running, and all tasks are not in the ready 
     * state, the idle task executes 
     * */ 

    idle();

//  for ( int i = 0; i < PPPLen; ++i )
//  {
//      set_interrupt(PPPMax[i]);
//      context_switch(PPP[i]);
//  }

}

void context_switch (void)
{
    OS_DI(); /* disable all interrupts */
//  save last process register values;
//  load next register values;
    OS_EI(); /* enable all interrupts */  
}
void OS_Abort()
{
//  EXIT();
}


//   A new process "p" is created to execute the parameterless
//   function "f" with an initial parameter "arg", which is retrieved
//   by a call to OS_GetParam().  If a new process cannot be
//   allocated, INVALIDPID is returned; otherwise, p's PID is
//   returned.  The created process will belong to scheduling "level",
//   which is DEVICE, SPORADIC or PERIODIC. If the process is PERIODIC, then
//   the "n" is a user-specified index (from 0 to MAXPROCESS-1) to be used
//   in the PPP[] array to specify its execution order.
//   Assumption: If "level" is SPORADIC, then "n" is ignored. If "level"
//   is DEVICE, then "n" is its rate.
/* Process Management primitives */  
PID  OS_Create(void (*f)(void), int arg, unsigned int level, unsigned int n)
{
    int iPID = INVALIDPID;
    if ( level == PERIODIC )
    {
        iPID = n;
        if ( n < MAXPROCESS && atProcesses[iPID].available == TRUE )
        {
            /**
             * Found the PID requested and it is available
             */
            atProcesses[iPID].level           = level;
            atProcesses[iPID].state           = NEW;
            atProcesses[iPID].arg             = arg;
            atProcesses[iPID].pcSp            = acWorkspaces + (WORKSPACE*iPID);
            atProcesses[iPID].pc              = (int) f;
            atProcesses[iPID].fifo            = INVALIDFIFO;
            atProcesses[iPID].available       = FALSE;
        }
        else /* PID is already taken by another process. */
        {
            iPID = INVALIDPID;
        }
    }
    else if ( level == SPORADIC || level == DEVICE )
    {
        for ( iPID = 0; iPID < MAXPROCESS; ++iPID )
        {
            if ( atProcesses[iPID].available == TRUE )
            {
                atProcesses[iPID].level           = level;
                atProcesses[iPID].state           = NEW;
                atProcesses[iPID].arg             = arg;
                atProcesses[iPID].pcSp             = acWorkspaces + (WORKSPACE*atProcesses[iPID].pid);
                atProcesses[iPID].pc              = (int) f;
                atProcesses[iPID].fifo             = INVALIDFIFO;
                atProcesses[iPID].available        = FALSE;

                if ( level == SPORADIC )
                {
                    atProcesses[iPID].frequency = n;
                    // put in sporadic queue
                }
                else if ( level == DEVICE )
                {
                    // put in device queue
                }
            }
        }
        /**
         * No free PCB was found.
         */
        if ( iPID >= MAXPROCESS )
        {
            iPID = INVALIDPID;
        }
    }
    else /* An invalid level was specified*/
    {
        OS_Abort();
    }
    return iPID;
}

/* FIFO primitives */

/**
 * INTERPROCESS COMMUNICATION:
 *    FIFOs are first-in-first-out bounded buffers. Elements are read in the same
 *    order as they were written. When writes overtake reads, the first unread
 *    element will be dropped. Thus, ordering is always preserved.
 *    "read" and "write" on FIFOs are atomic, i.e., they are indivisible, and
 *    they are non-blocking. All FIFOs are of the same size. All data elements
 *    are assumed to be unsigned int.
 */

/**   Initialize a new FIFO and returns a FIFO descriptor. It
 *    returns INVALIDFIFO when none is available.
 */
FIFO  OS_InitFiFo()
{
    FIFO fifo_id = INVALIDFIFO;
    return fifo_id;
}

/**
 *    Write a value "val" into the FIFO "f". A write always succeeds. When
 *    a FIFO is full, the first unread element is dropped.
 */
void  OS_Write( FIFO f, int val )
{
}


/**
 *    Return the first unread element in "f" if it is unavailable. If the FIFO is
 *    empty, it returns FALSE. Otherwise, it returns TRUE and the first
 *    unread element is copied into "val".
 */  
BOOL  OS_Read( FIFO f, int *val )
{
    return FALSE;
}
