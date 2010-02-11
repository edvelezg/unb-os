#include "os.h"
#include "lcd.h"
#include <stdlib.h>
#include <stdio.h>

void foo()
{
    /* do nothing */
}

/* main() can then create processes and initialize the PPP[] and PPPMax[] arrays */

void serial_print (const char *msg)
{
  while (*msg != 0)
    serial_send (*msg++);
}

int main (int argc, char *argv[])
{
    int my_var, my_int;
    /**
     *   The PPP[] array is a sequence of names which specifies the execution
     *   order of all PERIODIC processes. The name of every PERIODIC
     *   process must appear in PPP[] array at least once, but may be more 
     *   than once.
     */
//  OS_Init();


//PID  OS_Create(void (*f)(void), int arg, unsigned int level, unsigned int n)
//OS_Create(foo, 0, PERIODIC, 1);



    /* scheduling plan */
//  PPP[] = {IDLE, 1};
//  PPPMax[] = {3, 3};

    __asm__ __volatile__(
                       "ldaa      %0\n"
                       "ldab      %1\n"
                       "aba         \n"
                       "staa      %0\n"
                       : "=m"  (my_var)
                       : "m"  (my_int), "m" (my_var)
                       : "a", "b"                         /* no clobber-list */
                       );


    /* Configure the SCI to send at M6811_DEF_BAUD baud.  */
  _io_ports[M6811_BAUD] = M6811_DEF_BAUD;

  /* Setup character format 1 start, 8-bits, 1 stop.  */
  _io_ports[M6811_SCCR1] = 0;

  /* Enable receiver and transmitter.  */
  _io_ports[M6811_SCCR2] = M6811_TE | M6811_RE;

  serial_print ("Hello world!\n");

//  print("%d", my_int);
//  OS_Start();
    return 0;
}
