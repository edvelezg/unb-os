#include "os.h"

void foo()
{
    /* do nothing */
}

/* main() can then create processes and initialize the PPP[] and PPPMax[] arrays */

int main (int argc, char *argv[])
{
    /**
     *   The PPP[] array is a sequence of names which specifies the execution
     *   order of all PERIODIC processes. The name of every PERIODIC
     *   process must appear in PPP[] array at least once, but may be more 
     *   than once.
     */
//  OS_Init();


//PID  OS_Create(void (*f)(void), int arg, unsigned int level, unsigned int n)
//  OS_Create(foo, 0, PERIODIC, 1);

    /* scheduling plan */
//  PPP[] = {IDLE, 1};
//  PPPMax[] = {3, 3};
//  OS_Start();
    return 0;
}
