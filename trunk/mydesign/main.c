#include "ports.h"
#include "os.h"
#include "lcd.h"
#include "interrupts.h"

void serial_print (char *msg);
static inline void serial_send (char c);

void spo1()
{
    while ( TRUE )
    {
        sys_print_lcd("111111\n");
//      OS_Terminate();
    }
}

void spo2()
{
//  OS_Wait(1);
    while ( TRUE )
    {
        sys_print_lcd("222222\n");
//      OS_Terminate();
    }
}

void per1()
{
    while ( TRUE )
    {
        sys_print_lcd("AAAAAA\n");
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

void spo1sem()
{
    OS_Wait(2);
    while ( TRUE )
    {
        serial_print ("111111\n");
    }
}

void spo2sem()
{
    OS_Wait(2);
    while ( TRUE )
    {
        serial_print ("222222\n");
    }
}


void spo3sem()
{
    OS_Wait(2);
    while ( TRUE )
    {
        serial_print ("333333\n");
    }
}

void spo4sem()
{
    OS_Wait(2);
    while ( TRUE )
    {
        serial_print ("444444\n");
    }
}

void _Reset () {
    ///* Configure the SCI to send at M6811_DEF_BAUD baud.  */
    //_io_ports[M6811_BAUD] = M6811_DEF_BAUD;

    ///* Setup character format 1 start, 8-bits, 1 stop.  */
    //_io_ports[M6811_SCCR1] = 0;

    ///* Enable receiver and transmitter.  */
    //_io_ports[M6811_SCCR2] = M6811_TE | M6811_RE;

    _sys_init_lcd();

    //OS_Init();

    char *ken = "Ken!\0"; 
    char *joey = "Joey\0";
    unsigned int i;
    sys_print_lcd(ken);
    for ( i = 1; i != 0; i++ );
    for ( i = 1; i != 0; i++ );
    for ( i = 1; i != 0; i++ );
    for ( i = 1; i != 0; i++ );
    sys_print_lcd(joey);
    for ( i = 1; i != 0; i++ );
    for ( i = 1; i != 0; i++ );
    for ( i = 1; i != 0; i++ );
    for ( i = 1; i != 0; i++ );

    OS_Init();

    /* main() can then create processes and initialize the PPP[] and PPPMax[] arrays */

    OS_Create(spo1, 0, SPORADIC, 1);
    OS_Create(spo2, 0, SPORADIC, 1);
    OS_Create(per1, 0, PERIODIC, 'A');
    OS_Create(per2, 0, PERIODIC, 'B');
    OS_Create(dev1, 0, DEVICE, 6);
    OS_Create(dev2, 0, DEVICE, 6);

    PPP[0]      = 'A';
    PPP[1]      = 'B';
    PPP[2]      = IDLE;
    PPP[3]      = IDLE;
    PPPMax[0]   = 4;
    PPPMax[1]   = 4;
    PPPMax[2]   = 1;
    PPPMax[3]   = 2;
    PPPLen      = 4;

    OS_Start();
}

int main (void)
{
    RESETV = (unsigned int)&_Reset;     /* register the reset handler */
    while ( 1 ); 
    return 0; 
}
