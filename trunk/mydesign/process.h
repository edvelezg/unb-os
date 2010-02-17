#ifndef _PROCESS_H_
#define _PROCESS_H_

typedef enum 
{
    NEW, RUNNING, WAITING, READY, TERMINATED
} 
States;

/* process control block */
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

/* queue control block */
typedef struct queue_struct
{
    ProcCtrlBlock*  procRefs[MAXPROCESS];  /* message circular buffer */
    int             fillCount;         /* keeps track of the number of items */          
    int             next;              /* next spot to write in */
    int             first;             /* first element to read */

} ProcQueue;

/* software interrupt */
void SWI(void);
/* enqueue definition */
void Enqueue(ProcQueue* prq, ProcCtrlBlock* p);
/* dequeue definition */
BOOL Dequeue(ProcQueue *prq, ProcCtrlBlock** p);
void InitQueues();
void Schedule(void);
/* __attribute__ ((interrupt)) */
void context_switch (void);

#endif /* _PROCESS_H_ */
