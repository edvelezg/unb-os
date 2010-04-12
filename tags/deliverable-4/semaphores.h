#ifndef _SEMAPHORES_H_  
#define _SEMAPHORES_H_  

typedef struct semaphore_struct
{
	ProcCtrlBlock* 		procQueue[MAXPROCESS];  
	int			        procCount;			
	int 		        value;				
} Semaphore;

#endif
