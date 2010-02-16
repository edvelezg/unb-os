#ifndef _SEMAPHORES_H_  
#define _SEMAPHORES_H_  

typedef struct semaphore_struct
{
	ProcQueue	procQueue[MAXPROCESS];
	int 		count;

} Semaphore;

#endif
