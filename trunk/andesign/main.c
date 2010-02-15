#include "ports.h"
#include "os.h"

void serial_print (char *msg);
static inline void serial_send (char c);

void foo()
{
    /* do nothing */
}

void spo1()
{
    int i;
    while ( 1 )
    {
        serial_print ("I'm in Spo1()!\n");
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        OS_Yield();
    }
}

void spo2()
{
    int i;
    serial_print ("I'm in Spo2()!\n");
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    //OS_Yield();
}

void per1()
{
    int i;

    while ( 1 )
    {
        serial_print ("I'm in PerA() Part 1!\n");
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        OS_Yield();
        serial_print ("I'm in PerA Part 2()!\n");
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
    }
}

void per2()
{
    int i;
    serial_print ("I'm in PerB()!\n");
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    for ( i = 1; i != 0 ; ++i );
    //OS_Yield();
}

/* main() can then create processes and initialize the PPP[] and PPPMax[] arrays */

static inline void serial_send (char c)
{
    /* Wait until the SIO has finished to send the character.  */
    while ( !(_io_ports[M6811_SCSR] & M6811_TDRE) )
        continue;

    _io_ports[M6811_SCDR] = c;
    _io_ports[M6811_SCCR2] |= M6811_TE;
}

void serial_print (char *msg)
{
    while ( *msg != 0 )
        serial_send (*msg++);
}


int main (int argc, char *argv[])
{
    int my_var, my_int;

    /* Configure the SCI to send at M6811_DEF_BAUD baud.  */
    _io_ports[M6811_BAUD] = M6811_DEF_BAUD;

    /* Setup character format 1 start, 8-bits, 1 stop.  */
    _io_ports[M6811_SCCR1] = 0;

    /* Enable receiver and transmitter.  */
    _io_ports[M6811_SCCR2] = M6811_TE | M6811_RE;

    /**
     *   The PPP[] array is a sequence of names which specifies the execution
     *   order of all PERIODIC processes. The name of every PERIODIC
     *   process must appear in PPP[] array at least once, but may be more 
     *   than once.
     */
//  OS_DI();
    OS_Init();
    OS_Create(spo1, 0, SPORADIC, 1);
    OS_Create(spo2, 0, SPORADIC, 1);
    OS_Create(per1, 0, PERIODIC, 'A');
    OS_Create(per2, 0, PERIODIC, 'B');

    PPP[0] = IDLE;
    PPP[1] = 'A';
    PPP[2] = 'B';
    PPP[3] = IDLE;
    PPPMax[0] = 2;
    PPPMax[1] = 3;
    PPPMax[2] = 4;
    PPPMax[3] = 5;
    PPPLen = 4;

    OS_Start();

    return 0;
}
