#include "ports.h"
#include "os.h"

void serial_print (char *msg);
static inline void serial_send (char c);

//void idle(void)
//{
//    while ( TRUE )
//    {
//        serial_print("IIIIII\n");
//    }
//}

void spo1()
{
    int i;
    while ( TRUE )
    {
        serial_print ("111111\n");
    }
}

void spo2()
{
    int i;
    while ( TRUE )
    {
        serial_print ("222222\n");
    }
}

void per1()
{
    int i;

    while ( TRUE )
    {
        serial_print ("AAAAAA\n");
//      OS_Yield();
    }
}

void per2()
{
    while ( TRUE )
    {
        serial_print ("BBBBBB\n");
    }
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
    /* Configure the SCI to send at M6811_DEF_BAUD baud.  */
    _io_ports[M6811_BAUD] = M6811_DEF_BAUD;

    /* Setup character format 1 start, 8-bits, 1 stop.  */
    _io_ports[M6811_SCCR1] = 0;

    /* Enable receiver and transmitter.  */
    _io_ports[M6811_SCCR2] = M6811_TE | M6811_RE;

    OS_Init();
    OS_Create(spo1, 0, SPORADIC, 1);
    OS_Create(spo2, 0, SPORADIC, 1);
    OS_Create(per1, 0, PERIODIC, 'A');
    OS_Create(per2, 0, PERIODIC, 'B');

    PPP[0]      = IDLE;
    PPP[1]      = IDLE;
    PPP[2]      = 'A';
    PPP[3]      = 'B';
    PPPMax[0]   = 2;
    PPPMax[1]   = 5;
    PPPMax[2]   = 5;
    PPPMax[3]   = 4;
    PPPLen      = 4;

    OS_Start();

    return 0;
}