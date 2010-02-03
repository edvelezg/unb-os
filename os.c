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

enum States
{
    NEW, RUNNING, WAITING, READY, TERMINATED
};

struct PCB 
{
    int level;
    int state;
    int arg;
    int *stackPointer; // Memory-management info;

    // - the "function pointer to function" is the program counter. Just an
    // int is fine ???????????????
    int programCounter;
    void (*function)(void);
    //  scheduling info

};

struct PCB pcbArray[MAXPROCESS];



// main() can then create processes and initialize the PPP[] and PPPMax[] arrays
int main (int argc, char *argv[])
{
    /**
     *   The PPP[] array is a sequence of names which specifies the execution
     *   order of all PERIODIC processes. The name of every PERIODIC
     *   process must appear in PPP[] array at least once, but may be more 
     *   than once.
     */
    OS_Init();
    PPP[] = {1, 2, 1, 3, 2, 3}
    PPPMax[] = {3, 4, 5, 6, 7, 2}



    //   Before the call to OS_Start(), the only calls that may be placed to the
    //   OS are OS_Create(), OS_InitSem(), and OS_InitFiFo().
    OS_Create();
    OS_InitSem();
    OS_InitFiFo();
    OS_Start();
    return 0;
}

//   void OS_Init(void)
//   The function main() will be called first by crt11.s.  Before any calls
//   can be placed to the OS, main() must call OS_Init() to initialize the OS.
//   main() can then create processes and initialize the PPP[] and PPPMax[]
//   arrays. To boot the OS, main() must call OS_Start() which never returns.
//   Assumption: OS_Init() is called exactly once at boot time.
/* OS Initialization */
void OS_Init()
{

    // determine quantum .. i can hardcode this value because I am using the same HW
    setup an interrupt to increment a timer
    enable interrupts

    extern int PPPLen;          /* length of PPP[] */
    extern int PPP[];           /* PERIODIC process scheduling plan */
    extern int PPPMax[];        /* max CPU in msec of each process in PPP */

}
void OS_Start()
{
    for ( int i = 0; i < PPPLen; ++i )
    {
        set_interrupt(PPPMax[i]);
        run_process(PPP[i]);
    }

}

run_process ()
{
    disable interrupts.
    save last process register values;
    load next register values;
    re-enable interrupts
}
void OS_Abort()
{
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
    int myPID = INVALIDPID;
//  struct PCB p;
//  Do I point to function f with a pointer?

    if ( level == PERIODIC )
    {
        if ( n is not taken )
        {
            pcbArray[n].level = level;
            pcbArray[n].state = READY;
            pcbArray[n].stackPointer = memory location;
            pcbArray[n].function = f;
            pcbArray[n].programCounter = ;
            pcbArray[n].arg = arg;

            return INVALIDPID;
        }

    }
    else if ( level == SPORADIC )
    {
        ...
    }
    else if ( level == DEVICE )
    {
        ...
    }
    else
    {
        ...
    }
    return myPID;
}

void OS_Terminate(void);    
{
}

void OS_Yield(void);
{
}

int  OS_GetParam(void);  
{
    int retVal = 3;
    return retVal;
}

/* Semaphore primitives */
void OS_InitSem(int s, int n);
{
}

void OS_Wait(int s);
{
}

void OS_Signal(int s);
{
}

/* FIFO primitives */

/*
 * INTERPROCESS COMMUNICATION:
 *    FIFOs are first-in-first-out bounded buffers. Elements are read in the same
 *    order as they were written. When writes overtake reads, the first unread
 *    element will be dropped. Thus, ordering is always preserved.
 *    "read" and "write" on FIFOs are atomic, i.e., they are indivisible, and
 *    they are non-blocking. All FIFOs are of the same size. All data elements
 *    are assumed to be unsigned int.
 **  
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
