#ifndef _OS_H_  
#define _OS_H_  
  
/* KBK UNB/CS (Jan 24/2006) */
/* DO NOT EDIT THIS FILE! */

/*==================================================================  
 *             T Y P E S   &   C O N S T A N T S  
 *==================================================================  
 */  

  /* limits */
#define MAXPROCESS         16     /* max. # of processes supported */  
#define MAXFIFO            16     /* max. # of FIFOs supported */
#define MAXSEM             16     /* max. # of semaphores */
#define FIFOSIZE           8      /* max. # of data elements per FIFO */
#define WORKSPACE          512    /* workspace of each process in bytes */  

  /* invalid constants */
#define INVALIDPID         0      /* id of an invalid process */
#define INVALIDFIFO        0      /* an invalid FIFO descriptor */  

  /* scheduling levels */
#define SPORADIC           2      /* first-come-first-served, a periodic */
#define PERIODIC           1      /* cyclic, fixed-order, periodic */ 
#define DEVICE             0      /* time-driven cyclic device drivers */

  /* well-known process name */
#define IDLE               -1     /* name of an IDLE process */

  /* pre-defined constants */
#define TRUE               1      /* Boolean */
#define FALSE              0      /* Boolean */

  /* pre-defined types */
typedef unsigned int FIFO;
typedef unsigned int PID;  
typedef unsigned int BOOL;

  /* PERIODIC process scheduling plan */
extern int PPPLen;          /* length of PPP[] */
extern int PPP[];           /* PERIODIC process scheduling plan */
extern int PPPMax[];        /* max CPU in msec of each process in PPP */
  
/*==================================================================  
 *             A C C E S S    P R O C E D U R E S  
 *==================================================================  
 */  
  
  /* OS Initialization */
void OS_Init();
void OS_Start();
void OS_Abort();
  
  /* Process Management primitives */  
PID  OS_Create(void (*f)(void), int arg, unsigned int level, unsigned int n);
void OS_Terminate(void);    
void OS_Yield(void);
int  OS_GetParam(void);  

  /* Semaphore primitives */
void OS_InitSem(int s, int n);
void OS_Wait(int s);
void OS_Signal(int s);

  /* FIFO primitives */
FIFO  OS_InitFiFo();
void  OS_Write( FIFO f, int val );
BOOL  OS_Read( FIFO f, int *val );
   
/*==================================================================  
 *        S T A N D A R D   I N L I N E    P R O C E D U R E S  
 *==================================================================  
 */  
  
 /*   
  * inline assembly code to disable/enable maskable interrupts   
  * (N.B. Use with caution.)  
  */  
#define OS_DI()   /* asm(" sei ")  /* disable all interrupts */
#define OS_EI()   /* asm(" cli ")  /* enable all interrupts */
  

/*==================================================================  
 *          O S   I N T E R F A C E    S E M A N T I C S
 *==================================================================  
 */

/*  
 * GLOBAL ASSUMPTIONS:  
 *  - OC4 and SWI interrupts are reserved for use by the OS.
 *  - OC4 is used for pre-empting processes when their alloted time is used up.
 *  - SWI is used for kernel calls.
 *  - All runtime exceptions (where assumptions are violated) or other  
 *    unrecoverable errors get handled by calling OS_Abort().  
 *  - PPP[] and PPPMax[] must be of the same length, PPPLen.
 *  - Each entry in PPPMax[] must be between 1 and 10 msec inclusive.
 *  - PPPLen = 0 is the same as PPP = [IDLE] and PPPMax = [infinity].
 *  - All unspecified errors have undefined behaviours.
 *
 * SCHEDULING POLICY:   
 *
 *   There are three levels for processes: DEVICE, PERIODIC and SPORADIC. 
 *   SPORADIC processes are scheduled in a FCFS manner. PERIODIC processes
 *   are scheduled according to a cyclic fixed-order plan.  PERIODIC 
 *   processes have higher priority than SPORADIC processes. DEVICE
 *   processes have a predefined fixed rate of execution. They have the
 *   highest priority and pre-empt both SPORADIC and PERIODIC processes.
 *   Two DEVICE processes coincide on the time interval will be scheduled
 *   in an arbitrary order. We assume DEVICE processes have very short
 *   execution time.
 *
 * PERIODIC PROCESSES:
 *
 *   PERIODIC processes are scheduled according to a cyclic fixed-order
 *   scheduling plan, defined by the PPP[] array. When a PERIODIC process
 *   is created, it is assigned a name "n" (an index between 0 and MAXPROCESS-1).
 *   This name is fixed and can NEVER be changed again. The PID of this
 *   process is not the same as this name.  No two processes are allowed 
 *   to have the same name at the same time.  The same name may be reused 
 *   over time.
 *
 *   The PPP[] array is a sequence of names which specifies the execution
 *   order of all PERIODIC processes. The name of every PERIODIC
 *   process must appear in PPP[] array at least once, but may be more 
 *   than once.
 * 
 *   For example, if we create three PERIODIC processes with names A,B and 
 *   C out of three functions P(), Q() and R() respectively. Then, 
 *   PPP[] = { A, B, A, C } means executing A, then B, then A again, then 
 *   C, then A again, and so on. If P() terminates, but the name A is later 
 *   assigned to another instance of P() with a different PID, then A will 
 *   be executed again according to PPP[] order. In a sense, the PPP[] 
 *   specifies at least a single execution cycle of all PERIODIC processes. 
 *
 *   In addition, each PERIODIC process is assigned a maximum CPU time, 
 *   given by the array PPPMax[]. The process name IDLE is reserved for 
 *   introducing explicit CPU idle time. For example, 
 *         PPP[]    = { A, IDLE, B, IDLE, A,  C, IDLE };
 *         PPPMax[] = { 5, 2,    3, 3,    10, 7, 10 );
 *   After completing A within 5 msec, the processor idles 2 msec, 
 *   then starts B for 3 msec, then idles another 3 msec, then starts A again
 *   for 10 msec, then starts C for 7 msec, then idles 10 msec, then repeats 
 *   all over again. The total cycle time of all PERIODIC processes is thus
 *   40 msec. Each PERIODIC process is guaranteed to execute exactly according
 *   to the specified order and period of PPP[] and PPPMax[].
 *   By estimating the CPU execution time of each process we shall know 
 *   the approximate execution "cycle" time of all PERIODIC processes, 
 *   thus their best execution frequency/rate and response time. 
 *
 * CPU IDLE TIME:
 *
 *   The CPU is "idle" if:
 *       1.  a PERIODIC process yields/blocks before its max CPU time
 *           is exhausted; or
 *       2.  the IDLE process is the current PERIODIC process.
 *
 * SPORADIC PROCESSES:
 *   
 *   SPORADIC processes are allowed to run only during CPU idle time.  
 *   A ready SPORADIC process runs at the next earliest CPU idle time.  
 *   When a SPORADIC process is pre-empted, that is by the next PERIODIC 
 *   process, it re-enters its level at the front. When a SPORADIC process
 *   yields, or resumes after being unblocked, it re-enters its level
 *   at the end.
 *
 * IMPORTANT NOTE: 
 *       Once, OS_Start() has been called, PPP[] and PPPMax[] arrays must not 
 *       be changed.
 *
 * DEVICE PROCESSES:
 *    DEVICE level processes are designed to read/write I/O devices at specific
 *    data rates. They run for a very short time when activiated. They move
 *    data in/out of I/O devices. They must be run at a precise rate. When
 *    a DEVICE process is created, its "n" specifies its rate of execution,
 *    e.g., 20 (i.e., once every 20 milliseconds).
 *    Multiple DEVICE processes may collide at the same time interval.
 *    For example, one at every 20 msec and one at every 50 msec will collide
 *    once every 100 msec. Their execution order is not specified as long as
 *    both are executed eventually. Assume that DEVICE processes have very
 *    short execution time, then the jitter caused by collision should be 
 *    acceptable. DEVICE processes have the highest priority. When they 
 *    are ready, they are executed immediately, i.e., they pre-empt all
 *    other processes. All DEVICE processes run to completion, i.e., they
 *    run until they yield or terminate. When they yield, they will be
 *    resumed until next period.
 *
 * ACCESS CALLS:  
 *  
 * void OS_Init(void)
 *    The function main() will be called first by crt11.s.  Before any calls
 *    can be placed to the OS, main() must call OS_Init() to initialize the OS.
 *    main() can then create processes and initialize the PPP[] and PPPMax[] 
 *    arrays. To boot the OS, main() must call OS_Start() which never returns.
 *    Before the call to OS_Start(), the only calls that may be placed to the 
 *    OS are OS_Create(), OS_InitSem(), and OS_InitFiFo().
 *    Assumption: OS_Init() is called exactly once at boot time.  
 *
 * void OS_Start()
 *    OS_Start() will only be called once and only after the PPP[] and PPPMax[]
 *    arrays have been initialized and OS_Init() has been called. 
 *
 * void OS_Abort()
 *    stop the OS immediately due to an unrecoverable error.
 *  
 * PID OS_Create( void (*f)(void), int arg, unsigned int level,
 *                unsigned int n )
 *    A new process "p" is created to execute the parameterless
 *    function "f" with an initial parameter "arg", which is retrieved
 *    by a call to OS_GetParam().  If a new process cannot be
 *    allocated, INVALIDPID is returned; otherwise, p's PID is
 *    returned.  The created process will belong to scheduling "level",
 *    which is DEVICE, SPORADIC or PERIODIC. If the process is PERIODIC, then
 *    the "n" is a user-specified index (from 0 to MAXPROCESS-1) to be used
 *    in the PPP[] array to specify its execution order. 
 *    Assumption: If "level" is SPORADIC, then "n" is ignored. If "level"
 *                is DEVICE, then "n" is its rate.
 *
 * void OS_Terminate( void )  
 *    Terminate the calling process; when a process returns, i.e., it executes
 *    its last instruction in the associated function/code, it is
 *    automatically terminated.
 *  
 * void OS_Yield( void )
 *    Reschedule the calling process; that is, the calling process
 *    voluntarily gives up its share of the processor.
 *  
 * int OS_GetParam(void)  
 *    Retrieve the parameter ( "arg" ) provided by OS_Create().
 *
 * SEMAPHORES:
 *    A semaphore must be initialized by OS_InitSem() before it can be used.
 *    Processes waiting on a semaphore are resumed on a first-come first-served
 *    basis.
 *
 * void OS_InitSem(int s, int n);
 *    s is the identifier for the semaphore that is to be initialized. n is the
 *    number of processes that can be granted access to the critical region for
 *    this semaphore simultaneously. This function will initalize the internal
 *    data structures that are required before OS_Signal() and OS_Wait() can be
 *    invoked.
 *
 * void OS_Wait(int s);
 *    The invoking process is requesting to acquire the semaphore, s. If the
 *    internal counter allows, the process will continue executing after acquiring
 *    the semaphore. If not, the calling process will block and release the
 *    processor to the scheduler.
 *
 * void OS_Signal(int s);
 *    The invoking process will release the semaphore, if and only if the process
 *    is currently holding the semaphore. If a process is waiting on
 *    the semaphore, the process will be granted the semaphore and if appropriate
 *    the process will be given control of the processor, i.e. the waking process
 *    has a higher scheduling precedence than the current process. 
 *
 * INTERPROCESS COMMUNICATION:
 *    FIFOs are first-in-first-out bounded buffers. Elements are read in the same
 *    order as they were written. When writes overtake reads, the first unread
 *    element will be dropped. Thus, ordering is always preserved.
 *    "read" and "write" on FIFOs are atomic, i.e., they are indivisible, and
 *    they are non-blocking. All FIFOs are of the same size. All data elements
 *    are assumed to be unsigned int.
 *
 * FIFO  OS_InitFiFo()
 *    Initialize a new FIFO and returns a FIFO descriptor. It returns INVALIDFIFO
 *    when none is available.
 *
 * void  OS_Write( FIFO f, unsigned int val )
 *    Write a value "val" into the FIFO "f". A write always succeeds. When
 *    a FIFO is full, the first unread element is dropped.
 *
 * BOOL  OS_Read( FIFO f, unsigned int *val )
 *    Return the first unread element in "f" if it is unavailable. If the FIFO is
 *    empty, it returns FALSE. Otherwise, it returns TRUE and the first
 *    unread element is copied into "val".
 */  
  
#endif /* _OS_H_ */  
