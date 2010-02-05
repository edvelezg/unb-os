#include "os.h"
//#include <stdlib.h>

void foo()
{
    /* do nothing */
}

/* main() can then create processes and initialize the PPP[] and PPPMax[] arrays */

int main (int argc, char *argv[])
{
	  unsigned char my_int = 10, my_var = 15;

    /**
     *   The PPP[] array is a sequence of names which specifies the execution
     *   order of all PERIODIC processes. The name of every PERIODIC
     *   process must appear in PPP[] array at least once, but may be more 
     *   than once.
     */
  OS_Init();


//PID  OS_Create(void (*f)(void), int arg, unsigned int level, unsigned int n)
  OS_Create(foo, 0, PERIODIC, 1);

    /* scheduling plan */
//  PPP[] = {IDLE, 1};
//  PPPMax[] = {3, 3};



   __asm__ __volatile__(
                      "   ldaa %0 \n"
					  "   ldab %1 \n"
					  "	  aba   \n"
					  "   staa %0 \n"
                      : "=m"  (my_var)
                      : "m"  (my_int), "m" (my_var)
                      : "a", "b"                         /* no clobber-list */
                      );

   //print("%d", my_int);
   //OS_Start();
    return 0;
}
