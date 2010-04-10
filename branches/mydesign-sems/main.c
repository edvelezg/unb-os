#include "ports.h"
#include "os.h"
#include "interrupts.h"

#define EMPTYCOUNT      15;
#define FILLCOUNT       14;
#define MUTEX           13;

void serial_print (char *msg);
static inline void serial_send (char c);

void spo1sem()
{
    while ( TRUE )
    {
        OS_Wait(1);
        serial_print ("111111\n");
        OS_Signal(1);
        OS_Yield();
    }
}

void spo2sem()
{
    while ( TRUE )
    {
        OS_Wait(1);
        serial_print ("222222\n");
        OS_Signal(1);
        OS_Yield();
    }
}

void spo3sem()
{
    while ( TRUE )
    {
        OS_Wait(1);
        serial_print ("333333\n");
        OS_Signal(1);
        OS_Yield();
    }
}

void spo4sem()
{
    while ( TRUE )
    {
        OS_Wait(1);
        serial_print ("444444\n");
        OS_Signal(1);
        OS_Yield();
    }
}

void producer1()
{
    FIFO f = (FIFO)OS_GetParam();
    int j;
    int arr[8] = {0, 0, 1, 1, 1, 1, 0, 4};
    while ( TRUE )
    {
//      OS_Wait(13);
        for ( j = 0; j < FIFOSIZE; ++j ) {
            OS_Wait(15);
            OS_Write(f, arr[j]);
//          OS_Signal(14);
        }
//      OS_Signal(13);
        OS_Yield();
    }
    // write the value
    OS_Terminate();
}

void producer2()
{
    FIFO f = (FIFO)OS_GetParam();              
    int j;                                     
    int arr[8] = {0, 0, 2, 2, 2, 2, 0, 4};     
    while ( TRUE )                             
    {                                          
//      OS_Wait(13);
        for ( j = 0; j < FIFOSIZE; ++j ) {     
            OS_Wait(15);
            OS_Write(f, arr[j]);               
            OS_Signal(14);
        }                                      
//      OS_Signal(13);
        OS_Yield();
    }                                          
    // write the value                         
    OS_Terminate();                                    
}

void consumer()
{
    FIFO f = (FIFO)OS_GetParam();
    int j, value;
    while ( TRUE )
    {
//      OS_Wait(13);
        if ( OS_Read(f, &value) )
        {
//          OS_Wait(14);
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
            case 3:
                serial_print("3 ");
                break;
            case 4:
                serial_print("\n ");
                break;
            default:
                break;
            }
            OS_Signal(15); 
        }
//      OS_Signal(13);
        OS_Yield();
    }
//  OS_Terminate();
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


    OS_InitSem(15, 8);
    OS_InitSem(14, 0);
    OS_InitSem(13, 1);
    OS_InitSem(1, 1);
    FIFO f = OS_InitFiFo();
    // write the value
    PPP[0]      = IDLE;
    PPP[1]      = IDLE;
//  PPP[2]      = IDLE;
    PPPMax[0]   = 1;
    PPPMax[1]   = 1;
//  PPPMax[2]   = 1;
    PPPLen      = 2;
    OS_Create(spo1sem, f, SPORADIC, 1);
    OS_Create(spo2sem, f, SPORADIC, 1);
    OS_Create(spo3sem, f, SPORADIC, 1);
    OS_Create(spo4sem, f, SPORADIC, 1);

    OS_Create(consumer, f, DEVICE, 5);
    OS_Create(producer1, f, SPORADIC, 5);
//  OS_Create(producer2, f, SPORADIC, 5);

    OS_Start();

    return 0;
}
