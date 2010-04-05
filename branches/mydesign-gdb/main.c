#include "ports.h"
#include "os.h"

void serial_print (char *msg);
static inline void serial_send (char c);

void spo1sig()
{
    FIFO f = OS_GetParam();
    int j;
    int arr[8] = {0, 0, 2, 2, 2, 2, 0, 0};
    // write the value
    for ( j = 0; j < FIFOSIZE; ++j )
    {
        OS_Write(f, arr[j]);
    }
    OS_Terminate();
}

void spo2sig()
{
    FIFO f = OS_GetParam();
    int j, value;
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
    serial_print("\n");
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

    FIFO f = OS_InitFiFo();
    // write the value
    PPP[0]      = IDLE;
    PPP[1]      = IDLE;
    PPPMax[0]   = 1;
    PPPMax[1]   = 1;
    PPPLen      = 2;
    OS_Create(spo1sig, f, SPORADIC, 1);
    OS_Create(spo2sig, f, SPORADIC, 1);

    OS_Start();

    return 0;
}
