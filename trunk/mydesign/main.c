/** Prints correctly to screen now yay **/

#include "ports.h"
#include "os.h"
#include "lcd.h"
#include "interrupts.h"

void serial_print (char *msg);
static inline void serial_send (char c);
void beep(int value);

void beep(int value)
{
	int j;
	char i, k;
	for (j = 0; j < 128; ++j)
	{
		B_SET(_io_ports[M6811_PORTA], 3);
		for (k = 0; k < value; ++k)
			for ( i = 1 ; i != 128; ++i );
		B_UNSET(_io_ports[M6811_PORTA], 3);
		for (k = 0; k < value; ++k)
			for ( i = 1 ; i != 128; ++i );
	}
}

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
        //OS_Wait(1);
        sys_print_lcd("444444\0");
        //OS_Signal(1);
        //OS_Yield();
    }
}

void producer1()
{
    FIFO f = (FIFO)OS_GetParam();
    int j;
    int arr[8] = {0, 1, 2, 3, 3, 2, 1, 0};
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
    int i;
    unsigned int a = 0;
    unsigned int b = 0;
    char *str = "    \0";
    while ( TRUE )
    {
        b = a;
        str[2] = (b % 10) + '0';
        b /= 10;
        str[1] = (b % 10) + '0';
        b /= 10;
        str[0] = (b % 10) + '0';
        sys_print_lcd(str);
        ++a;
    }
    OS_Terminate();
}

void per2()
{
    static int j = 0;
    unsigned int *myP;
    char *str = "       \0";
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
        sys_print_lcd (str);
        ++j;
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
			beep(value + 1);
            switch ( value )
            {
            case 0:
				sys_print_lcd("0 \0");
                break;
            case 1:
                sys_print_lcd("1 \0");
                break;
            case 2:
                sys_print_lcd("2 \0");
                break;
            case 3:
                sys_print_lcd("3 \0");
                break;
            case 4:
                sys_print_lcd("4 \0");
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

void dev3()
{
	char i;
    while ( TRUE )
    {
		_io_ports[M6811_DDRD] = 0xFF; /* it sets the data direction to output for port d for all the pins*/
		B_SET(_io_ports[M6811_PORTD],4); /* it sets the data direction to output for port d for all the pins*/
		B_SET(_io_ports[M6811_PORTD],5); /* it sets the data direction to output for port d for all the pins*/
		//_io_ports[M6811_PORTD] = 0xFF; /* I want 4 and 5 */
		
		/* Turns on right and left motors */
		//B_SET(_io_ports[M6811_PORTA], 5);  // right
		//B_SET(_io_ports[M6811_PORTA], 6);	// left
		_io_ports[M6811_PORTA] = 0xFF; /* I want 4 and 5 */

		for ( i = 1 ; i != 0; ++i );
		for ( i = 1 ; i != 0; ++i );
		for ( i = 1 ; i != 0; ++i );
		//for ( i = 1 ; i != 0; ++i );
		//for ( i = 1 ; i != 0; ++i );

		B_UNSET(_io_ports[M6811_PORTA], 5);
		B_UNSET(_io_ports[M6811_PORTA], 6);
		OS_Yield();
	}
}

void dev2()
{
    while ( TRUE )
    {
		int j;
		char i;
        //sys_print_lcd("AAAAAA\n");
		for (j = 0; j < 255; ++j)
		{
			B_SET(_io_ports[M6811_PORTA], 3);
			for ( i = 1 ; i != 0; ++i );
			for ( i = 1 ; i != 0; ++i );
			B_UNSET(_io_ports[M6811_PORTA], 3);
			for ( i = 1 ; i != 0; ++i );
			for ( i = 1 ; i != 0; ++i );
		}
		OS_Yield();
    }
}

void senseLight()
{
	char *a = "-------\0";
	int i;
	int lightValue;
	while (TRUE)
	{
		B_SET(_io_ports[OPTION], 7); // switch port e to analog mode
		OS_Yield();
	
		_io_ports[ADCTL] = 0; // right photocell is pin 0, so you set it to 0.

		while (!(_io_ports[ADCTL] & 128)) // waits bit 7 to light up
		{	
			OS_Yield();
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

    sys_print_lcd("SemOSXTreme4!\0");
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
	PPP[2]      = 'A';
    PPPMax[0]   = 1;
    PPPMax[1]   = 1;
	PPPMax[2]   = 1;
    PPPLen      = 3;
    //OS_Create(spo1sem, f, SPORADIC, 1);
    //OS_Create(spo2sem, f, SPORADIC, 1);
    //OS_Create(spo3sem, f, SPORADIC, 1);
    OS_Create(per2, f, PERIODIC, 'A');

    OS_Create(consumer, f, DEVICE, 2);
    OS_Create(senseLight, f, DEVICE, 2);
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
