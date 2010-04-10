/** Prints correctly to screen now yay **/

#include "ports.h"
#include "os.h"
#include "lcd.h"
#include "interrupts.h"

void serial_print (char *msg);
static inline void serial_send (char c);

void spo1sem()
{
    while ( TRUE )
    {
        OS_Wait(1);
        sys_print_lcd("111111\0");
        OS_Signal(1);
        OS_Yield();
    }
}

void spo2sem()
{
    while ( TRUE )
    {
        OS_Wait(1);
        sys_print_lcd("222222\0");
        OS_Signal(1);
        OS_Yield();
    }
}


void spo3sem()
{
    while ( TRUE )
    {
        OS_Wait(1);
        sys_print_lcd("333333\0");
        OS_Signal(1);
        OS_Yield();
    }
}

void spo4sem()
{
    while ( TRUE )
    {
        OS_Wait(1);
        sys_print_lcd("444444\0");
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

void per1()
{
    while ( TRUE )
    {
		char i;
        //sys_print_lcd("AAAAAA\n");
		B_SET(_io_ports[M6811_PORTA], 3);
		for ( i = 1 ; i != 0; ++i );
		B_UNSET(_io_ports[M6811_PORTA], 3);
		for ( i = 1 ; i != 0; ++i );
//      OS_Terminate();
    }
}

void per2()
{
    while ( TRUE )
    {
        sys_print_lcd("BBBBBB\n");
//      OS_Terminate();
    }
}

void dev1()
{
    int i;
    unsigned int a = 0;
    unsigned int b = 0;
    char *str = "   \n";
    while ( TRUE )
    {
        serial_print ("I'm a device process\n");
        serial_print ("That counts\n");
        b = a;
        str[2] = (b % 10) + '0';
        b /= 10;
        str[1] = (b % 10) + '0';
        b /= 10;
        str[0] = (b % 10) + '0';
        serial_print (str);
        ++a;
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        OS_Yield(); /* must yield or terminate, if not it won't be interrupted.*/
    }
    OS_Terminate();
}

void dev2()
{
    static int j = 0;
    unsigned int *myP;
    char *str = "      ";
    unsigned int i;

    myP = (unsigned int *)&_io_ports[TCNT];

    while ( j <= 5 )
    {
        i = *myP;
        str[5] = (i % 10) + '0';
        i /= 10;
        str[4] = (i % 10) + '0';
        i /= 10;
        str[3] = (i % 10) + '0';
        i /= 10;
        str[2] = (i % 10) + '0';
        i /= 10;
        str[1] = (i % 10) + '0';
        serial_print ("TCNTs value is:\n");
        serial_print (str);
        serial_print ("\n");
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        for ( i = 1; i != 0 ; ++i );
        ++j;
        OS_Yield();  /* must yield or terminate, if not it won't be interrupted.*/
    }
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

void _Reset () {

    _sys_init_lcd();

    unsigned int i;

    sys_print_lcd("SemOSXtreme!\0");
    for ( i = 1; i != 0; i++ );

    OS_Init();

    /* main() can then create processes and initialize the PPP[] and PPPMax[] arrays */

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
}

int main (void)
{
    RESETV = (unsigned int)&_Reset;     /* register the reset handler */
    //while ( 1 ); 
    return 0; 
}
