#include "ports.h"
#include "os.h"

void serial_print (char *msg);
static inline void serial_send (char c);

void spo1sem()
{
    while ( TRUE )
    {
        OS_Wait(1);
        serial_print ("111111\n");
    }
}

void spo2sem()
{
    while ( TRUE )
    {
        OS_Wait(1);
        serial_print ("222222\n");
    }
}

void spo1sig()
{
    FIFO f = OS_GetParam();
    int j;
    int arr[8] = {0, 0, 2, 2, 2, 2, 0, 0};
    while ( TRUE )
    {
        OS_Wait(2);
        for ( j = 0; j < FIFOSIZE; ++j )
        {
            OS_Write(f, arr[j]);
        }
        OS_Signal(2);
    }
    // write the value
    OS_Terminate();
}

void spo2sig()
{
    FIFO f = OS_GetParam();
    int j, value;
    while ( TRUE )
    {
        OS_Wait(2);
        for ( j = 0; j < FIFOSIZE; ++j )
        {
            OS_Read(f, &value);
            switch ( value )
            {
            case 0:
                serial_print("0 ");
                break;
            case 1:
                serial_print("1 ");
                break;
            case 2:
                serial_print("2 ");
                break;
            default:
                break;
            }
        }
        OS_Signal(2);
        serial_print("\n");
    }
    OS_Terminate();
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

    /* Write & Read multiple Values Test */


    OS_InitSem(1, 1);

    PPP[0]      = IDLE;
    PPP[1]      = IDLE;
    PPPMax[0]   = 1;
    PPPMax[1]   = 1;
    PPPLen      = 2;
    OS_Create(spo1sem, 0, SPORADIC, 1);
    OS_Create(spo2sem, 0, SPORADIC, 1);


    /* [#t] Mutex Signal Primitive Test */

//
//  PPP[0]      = IDLE;
//  PPP[1]      = IDLE;
//  PPPMax[0]   = 1;
//  PPPMax[1]   = 1;
//  PPPLen      = 2;
//  OS_Create(spo1sig, 0, SPORADIC, 1);
//  OS_Create(spo2sig, 0, SPORADIC, 1);


    /* [#t] Semaphores with value 3 Wait Primitive Test */
//  
//  OS_InitSem(2, 3);
//
//  PPP[0]      = IDLE;
//  PPP[1]      = IDLE;
//  PPPMax[0]   = 1;
//  PPPMax[1]   = 1;
//  PPPLen      = 2;
//  OS_Create(spo1semsig, 0, SPORADIC, 1);
//  OS_Create(spo2semsig, 0, SPORADIC, 1);
//  OS_Create(spo3semsig, 0, SPORADIC, 1);
//  OS_Create(spo4semsig, 0, SPORADIC, 1);
//
//
//  OS_Start();


//  OS_InitSem(2, 1);
//  OS_InitSem(3, 1);
//  FIFO f = OS_InitFiFo();
//  // write the value
//  PPP[0]      = IDLE;
//  PPP[1]      = IDLE;
//  PPPMax[0]   = 1;
//  PPPMax[1]   = 1;
//  PPPLen      = 2;
//  OS_Create(spo1sig, f, SPORADIC, 1);
//  OS_Create(spo2sig, f, SPORADIC, 1);

    OS_Start();

    return 0;
}
