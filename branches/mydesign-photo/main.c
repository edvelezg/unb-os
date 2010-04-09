/** Prints correctly to screen now yay **/

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
        //sys_print_lcd("111111\n");
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

void dev1()
{
	
	char *a = "--------";
	int i;
	int lightValue;
	while (TRUE)
	{
		B_SET(_io_ports[OPTION], 7); // switch port e to analog mode
		OS_Yield();
	
		

		_io_ports[ADCTL] = 0; // right photocell is pin 0, so you set it to 0.

		while (!(_io_ports[ADCTL] & 128)) // waits bit 7 to light up
		{	
			//sys_print_lcd("I'm here");
			//OS_Yield();
		}

		lightValue = _io_ports[ADR1];
		a[2] = lightValue%10 + '0';
		lightValue /= 10;
		a[1] = (lightValue)%10 + '0';
		lightValue /= 10;
		a[0] = (lightValue)%10 + '0';

		sys_print_lcd(a);
		//OS_Yield();
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

    _sys_init_lcd();

    //OS_Init();

//  char *ken = "Ken!\0";
//  char *joey = "Joey\0";
    unsigned int i;
//  sys_print_lcd(ken);
    sys_print_lcd("Ken!\0");
    for ( i = 1; i != 0; i++ );

//  sys_print_lcd(joey);
    sys_print_lcd("Joey!\0");
    for ( i = 1; i != 0; i++ );

    OS_Init();

    /* main() can then create processes and initialize the PPP[] and PPPMax[] arrays */

    OS_Create(spo1, 0, SPORADIC, 1);
    OS_Create(dev1, 0, DEVICE, 10);
    //OS_Create(dev1, 0, DEVICE, 6);
    //OS_Create(dev2, 0, DEVICE, 6);

    PPP[0]      = IDLE;
	PPP[1]      = IDLE;
    PPPMax[0]   = 1;
    PPPMax[1]   = 1;
    PPPLen      = 2;

    OS_Start();
}

int main (void)
{
    RESETV = (unsigned int)&_Reset;     /* register the reset handler */
    //while ( 1 ); 
    return 0; 
}
