#include "ports.h"
#include "os.h"

void serial_print (char *msg);
static inline void serial_send (char c);

void spo1sem()
{
    OS_Wait(1);
    while ( TRUE )
    {
        serial_print ("111111\n");
    }
}

void spo2sem()
{
    OS_Wait(1);
    while ( TRUE )
    {
        serial_print ("222222\n");
    }
}


void spo3sem()
{
    OS_Wait(1);
    while ( TRUE )
    {
        serial_print ("333333\n");
    }
}

void spo4sem()
{
    OS_Wait(1);
    while ( TRUE )
    {
        serial_print ("444444\n");
    }
}

void spo1semsig()
{
    while ( TRUE )
    {
        OS_Wait(1);
        serial_print ("111111\n");
        OS_Signal(1);
    }
}

void spo2semsig()
{
    while ( TRUE )
    {
        OS_Wait(1);
        serial_print ("222222\n");
        OS_Signal(1);
    }
}


void spo3semsig()
{
    while ( TRUE )
    {
        OS_Wait(1);
        serial_print ("333333\n");
        OS_Signal(1);
    }
}

void spo4semsig()
{
    while ( TRUE )
    {
        OS_Wait(1);
        serial_print ("444444\n");
        OS_Signal(1);
    }
}

void spo1sig()
{
    while ( TRUE )
    {
        OS_Wait(1);
        serial_print ("111111\n");
        OS_Signal(1);
    }
}

void spo2sig()
{
    while ( TRUE )
    {
        OS_Wait(1);
        serial_print ("222222\n");
        OS_Signal(1);
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

    /* Generic Test */

//  OS_Create(spo1, 0, SPORADIC, 1);
//  OS_Create(spo2, 0, SPORADIC, 1);
//  OS_Create(per1, 0, PERIODIC, 'A');
//  OS_Create(per2, 0, PERIODIC, 'B');
//  OS_Create(dev1, 0, DEVICE, 6);
//  OS_Create(dev2, 0, DEVICE, 6);
//
//  PPP[0]      = 'A';
//  PPP[1]      = 'B';
//  PPP[2]      = IDLE;
//  PPP[3]      = IDLE;
//  PPPMax[0]   = 4;
//  PPPMax[1]   = 4;
//  PPPMax[2]   = 1;
//  PPPMax[3]   = 2;
//  PPPLen      = 4;

    /* [#t] Mutex Wait Primitive Test */

//  OS_InitSem(1, 1);
//
//  PPP[0]      = IDLE;
//  PPP[1]      = IDLE;
//  PPPMax[0]   = 1;
//  PPPMax[1]   = 1;
//  PPPLen      = 2;
//  OS_Create(spo1sem, 0, SPORADIC, 1);
//  OS_Create(spo2sem, 0, SPORADIC, 1);


    /* [#t] Mutex Signal Primitive Test */

    OS_InitSem(1, 1);

    PPP[0]      = IDLE;
    PPP[1]      = IDLE;
    PPPMax[0]   = 1;
    PPPMax[1]   = 1;
    PPPLen      = 2;
    OS_Create(spo1sig, 0, SPORADIC, 1);
    OS_Create(spo2sig, 0, SPORADIC, 1);


    OS_Start();

    return 0;
}
