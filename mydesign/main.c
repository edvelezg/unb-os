#include "ports.h"
#include "os.h"

void serial_print (char *msg);
static inline void serial_send (char c);

void spo1()
{
    while ( TRUE )
    {
        serial_print ("111111\n");
    }
    serial_print ("111111\n");
    serial_print ("111111\n");
    serial_print ("111111\n");
    serial_print ("111111\n");
    serial_print ("111111\n");
    OS_Terminate();
}

void spo2()
{
//  while ( TRUE )
//  {
//      serial_print ("222222\n");
//  }
    serial_print ("222222\n");
    serial_print ("222222\n");
    serial_print ("222222\n");
    serial_print ("222222\n");
    serial_print ("222222\n");
    serial_print ("222222\n");
    serial_print ("222222\n");
    serial_print ("222222\n");
    OS_Terminate();
}

void per1()
{
    /* simple counter program */
    unsigned int a = 0;
    unsigned int b = 0;
    char *str = "   \n";
    while ( TRUE )
    {
        b = a;
        str[2] = (b % 10) + '0';
        b /= 10;
        str[1] = (b % 10) + '0';
        b /= 10;
        str[0] = (b % 10) + '0';
        serial_print (str);
        ++a;
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

void dev1()
{
    int i;
    while ( TRUE )
    {
        serial_print ("I'm a device process\n");
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        OS_Yield();
    }
}

void dev2()
{
    int i;
    while ( TRUE )
    {
        serial_print ("I'm a DEVICE PROCESS 2\n");
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        OS_Yield();
    }
}

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

    /* main() can then create processes and initialize the PPP[] and PPPMax[] arrays */

    OS_Create(spo1, 0, SPORADIC, 1);
    OS_Create(spo2, 0, SPORADIC, 1);
    OS_Create(per1, 0, PERIODIC, 'A');
    OS_Create(per2, 0, PERIODIC, 'B');
    OS_Create(dev1, 0, DEVICE, 6);
    OS_Create(dev2, 0, DEVICE, 18);

    PPP[0]      = 'A';
    PPP[1]      = 'B';
    PPP[2]      = IDLE;
    PPP[3]      = IDLE;
    PPPMax[0]   = 3;
    PPPMax[1]   = 3;
    PPPMax[2]   = 6;
    PPPMax[3]   = 6;
    PPPLen      = 4;

    OS_Start();

    return 0;
}
