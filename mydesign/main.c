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

void spo0()
{
    FIFO f = (FIFO)OS_GetParam();
    int j;
    int arr[8] = {1, 2, 3, 4, 4, 3, 2, 1};
    for ( j = 0; j < FIFOSIZE; ++j )
    {
        OS_Wait(15);
        OS_Write(f, arr[j]);
        //OS_Signal(14);
    }
    OS_Terminate();
}

void spo1()
{
    FIFO f = (FIFO)OS_GetParam();
    int j;
    int arr[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    for ( j = 0; j < FIFOSIZE; ++j )
    {
        OS_Wait(15);
        OS_Write(f, arr[j]);
        //OS_Signal(14);
    }
    OS_Terminate();
}

void spo2()
{
    FIFO f     = (FIFO)OS_GetParam();
    int j;
    int arr[8] = {7, 6, 5, 4, 3, 2, 1, 0};
    for ( j = 0; j < FIFOSIZE; ++j )
    {
        OS_Wait(15);
        OS_Write(f, arr[j]);
        //OS_Signal(14);
    }
    OS_Terminate();
}

void spo3()
{
    FIFO f = (FIFO)OS_GetParam();
    int j;
    int arr[8] = {1, 6, 1, 6, 1, 6, 1, 6};
    for ( j = 0; j < FIFOSIZE; ++j )
    {
        OS_Wait(15);
        OS_Write(f, arr[j]);
        //OS_Signal(14);
    }
    OS_Terminate();
}

void producer1()
{

    FIFO f = (FIFO)OS_GetParam();

	OS_Wait(1);
    OS_Create(spo0, f, SPORADIC, 1);

    while ( TRUE )
    {
        OS_Wait(1);
        OS_Create(spo1, f, SPORADIC, 1);

        OS_Wait(1);
        OS_Create(spo2, f, SPORADIC, 1);

        OS_Wait(1);
        OS_Create(spo3, f, SPORADIC, 1);
    }
}

void perA()
{
    static int j = 0;
    unsigned int *myP;
    char *str = "       \0";
    unsigned int i;

    myP = (unsigned int *)&_io_ports[TCNT];

    while ( TRUE )
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
}

void consumer()
{
    FIFO f = (FIFO)OS_GetParam();
    int j, value;
    while ( TRUE )
    {
        while ( OS_Read(f, &value) )
        {
			//OS_Wait(14);
            switch ( value )
            {
            case 0:
				beep(value + 1);
				//sys_print_lcd("0 \0");
				break;
            case 1:
                beep(value + 1);
				//sys_print_lcd("1 \0");
                break;
            case 2:
                beep(value + 1);
				//sys_print_lcd("2 \0");
                break;
            case 3:
                beep(value + 1);
				//sys_print_lcd("3 \0");
                break;
            case 4:
                beep(value + 1);
				//sys_print_lcd("4 \0");
                break;
            case 5:
                beep(value + 1);
				//sys_print_lcd("5 \0");
                break;
            case 6:
                beep(value + 1);
				//sys_print_lcd("6 \0");
                break;
            case 7:
                beep(value + 1);
				//sys_print_lcd("7 \0");
                break;
            default:
                break;
            }
			for ( j = 1 ; j < 125; ++j );
            OS_Signal(15); 
			//OS_Yield();
        }
        OS_Signal(1);
        OS_Yield();
    }
}

void perB()
{
	char i;
    while ( TRUE )
    {
		_io_ports[M6811_DDRD] = 0xFF; /* it sets the data direction to output for port d for all the pins*/
        _io_ports[M6811_PORTD] = 0xFF; /* I want 4 and 5 */
//  	B_SET(_io_ports[M6811_PORTD],4); /* it sets the data direction to output for port d for all the pins*/
//  	B_SET(_io_ports[M6811_PORTD],5); /* it sets the data direction to output for port d for all the pins*/
		
		/* Turns on right and left motors */
		//B_SET(_io_ports[M6811_PORTA], 5);  // right
		//B_SET(_io_ports[M6811_PORTA], 6);	// left
		_io_ports[M6811_PORTA] = 0xFF; /* I want 4 and 5 */
        B_SET(_io_ports[M6811_PORTA], 6);
        B_SET(_io_ports[M6811_PORTA], 5);

		for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );
        for ( i = 1 ; i != 0; ++i );

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

//void beepToDarkness( int value )
//{
//
//    while ( value >= 150 )
//    {
//		int i;
//		for ( i = 1 ; i != 0; ++i );
//        if ( value >= 150 && value < 190 )
//        {
//            beep(5);
//			sys_print_lcd("5 \0");
//        }
//        else if ( value >= 190 && value < 220 )
//        {
//            beep(4);
//			sys_print_lcd("4 \0");
//        }
//        else if ( value >= 220 && value < 230 )
//        {
//            beep(3);
//			sys_print_lcd("3 \0");
//        }
//        else if ( value >= 220 && value < 230 )
//        {
//            beep(2);
//			sys_print_lcd("2 \0");
//        }
//        else if ( value >= 230 )
//        {
//            beep(1);
//			sys_print_lcd("1 \0");
//        }
//    }
//}

void senseLight()
{
	char *a = "-------\0";
	int printVal;
	char lightValue = 30;
	while (TRUE)
	{
		B_SET(_io_ports[OPTION], 7); // switch port e to analog mode
		if ( lightValue < 150 )
		{
			OS_Yield();
		}
		else
		{
		}
	
		_io_ports[ADCTL] = 0; // right photocell is pin 0, so you set it to 0.

		while (!(_io_ports[ADCTL] & 128)) // waits for bit 7 to light up
		{	
			if ( lightValue < 150 )
			{
				OS_Yield();
			}
			else
			{
			}
		}
		
		if (lightValue >= 150)
		{
			beep(5);
		}
		
		lightValue = _io_ports[ADR1];
		printVal = lightValue;

		a[2] = (printVal%10) + '0';
		printVal /= 10;
		a[1] = (printVal%10) + '0';
		printVal /= 10;
		a[0] = (printVal%10) + '0';
		sys_print_lcd(a);
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

    sys_print_lcd("SuperSemOSXTrEme4!\0");
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
    //OS_Create(spo4sem, 0, SPORADIC, 1);
    OS_Create(perA, 0, PERIODIC, 'A');
    OS_Create(perB, 0, DEVICE, 5);
    OS_Create(consumer, f, DEVICE, 5);
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
