#include "debug.h"
#include "ports.h"

#define MAX_DB_STR 256

extern volatile unsigned char _io_ports[];

unsigned int DB_FIRST_RUN = 1;
unsigned int DB_PRINT_ZEROES = 1; /* set to 0 for large data ranges */


/*****************************************************************************/
/* Part 2:  68HC11 SIO Operations. */
/*! Send the character on the serial line.*/
/*****************************************************************************/
void serial_send( char c )
{

  /* Wait until the SIO has finished to send the character.  */
  while ( !( _io_ports[ M6811_SCSR ] & M6811_TDRE ) )
    continue;

	_io_ports[ M6811_SCDR ] = c;
	_io_ports[ M6811_SCCR2 ] |= M6811_TE;
}

/*****************************************************************************/


void init_serial()
{
	_io_ports[ M6811_BAUD ] = M6811_DEF_BAUD;       /* Configure the SCI to send at M6811_DEF_BAUD baud.  */
  _io_ports[ M6811_SCCR1 ] = 0;                   /* Setup character format 1 start, 8-bits, 1 stop.  */
  _io_ports[ M6811_SCCR2 ] = M6811_TE | M6811_RE; /* Enable receiver and transmitter.  */

	db_lock = 0;
}


void serial_print( char *msg )
{
	unsigned i;

	if ( DB_FIRST_RUN )
	{
	  DB_FIRST_RUN = 0;
		init_serial();
	}

	//__asm__ __volatile( " SEI" );

	for( i = 0; i < MAX_STRING_LENGTH && msg[ i ] != 0; i++ )
	  serial_send ( msg[ i ] );

	//__asm__ __volatile( " CLI" );
}

void serial_print_hex( unsigned int hex )
{
	char msg[ MAX_STRING_LENGTH ];

	int_to_str( hex, msg );
	serial_print( msg );
}

/* converts value into a string representing the hex value */
int int_to_str( unsigned int value, char* str )
{
  short i = 0;
  unsigned int divider = 0xF;  // cannot use 0x10 because of poss. overflow

  while ( divider < value && i < MAX_INT_LENGTH )
  {
    i++;
    divider = divider << 4;
    divider += 0xF; // 0xF, 0xFF, 0xFFF...
  }

  if ( ( ( i + 1 ) & 1 ) == 1 ) // i is even, single digit
    i++;                        // so we will leftpad str with a 0

  if ( i + 1 >= MAX_INT_LENGTH )
    return 0;


  str[ i + 1 ] = 0;

  while ( i >= 0 )
  {
    str[ i ] = (unsigned char)(  ( value & 0xF ) + 48 );
    value = value >> 4; // divide value by 16
    i--;
  }

  i = 0;
  while ( str[ i ] != 0 )
  {
    if ( str[ i ] > 57 )
    	str[ i ] += (char)7;
    i++;
  }
  return 1;
}

void print_var( char* name, void* mem )
{
	serial_print( "  " );
	serial_print( name );
	serial_print( " = " );
	serial_print_hex( *(unsigned int*)mem );
	serial_print( "  [ " );
	serial_print_hex( (unsigned int)mem );
	serial_print( " ]\n" );
}


void mem_dump( unsigned int mem_start, unsigned int mem_end )
{
	long i, k;
	unsigned short found_non_zero = 0;
	unsigned char buffer[ 0x10 ];

	serial_print( "Memdump [ " );
	serial_print_hex( mem_start );
	if ( mem_end > mem_start )
	{
		serial_print( ".." );
		serial_print_hex( mem_end );
	}
	serial_print( " ]:\n" );
	serial_print( "          0  1  2  3    4  5  6  7    8  9  A  B    C  D  E  F\n" );

	mem_start &= 0xFFF0;
	mem_end |= 0x000F;

	for ( i = mem_start; i <= mem_end; i++ )
	{
		buffer[ i & 0xF ] = *(unsigned char*)i;
		if ( buffer[ i & 0xF ] != 0 )
			found_non_zero = 1;

		if ( ( i & 0x000F ) == 0x000F ) /* last digit of i is F */
		{

		  /* DB_PRINT_ZEROES set to 0 will skip lines of all 0's */
			if ( found_non_zero || DB_PRINT_ZEROES )
			{
				found_non_zero = 0;
				serial_print( "  " );
				if ( i < 0x100 )
					serial_print( "00" );
				serial_print_hex( i - 0xF );
				serial_print( " |" );

				k = 0;
				while ( k <= 0xF )
				{
					serial_print( " " );
					if ( buffer[ k ] == 0 )
						serial_print( " ." );
					else
						serial_print_hex( buffer[ k ] );

					if ( ( k & 0x3 ) == 0x3 )
						serial_print( " |" );
					k++;
				}
				serial_print( "\n" );
			}
		}
	}
	serial_print( "Memdump end\n" );
}



void stack_dump( unsigned int length )
{
  volatile unsigned int sp;

	__asm__ __volatile__("STS %0" :  : "m" ( sp ) );
	if ( length > sp )
    length = sp;
	serial_print( "Stack Pointer ( " );
	serial_print_hex( sp + STACK_FUNC_OFFSET );
	serial_print( " ) " );

	mem_dump( sp + STACK_FUNC_OFFSET, sp + STACK_FUNC_OFFSET + length );
}

void stack_dump_text( unsigned int length )
{
  volatile unsigned char *sp;
	unsigned char buffer[ MAX_STRING_LENGTH ];
	unsigned int i;

	__asm__ __volatile__("STS %0" :  : "m" ( sp ) );
	if ( length > (unsigned int)sp )
    length = (unsigned int)sp;
	serial_print( "Stack Pointer ( " );
	serial_print_hex( (unsigned int)sp + STACK_FUNC_OFFSET );
	serial_print( " ): " );

	for ( i = 0; i < length; i++ )
	{
	  if ( *sp >= 0x20 && *sp < 0x7F )
			buffer[ i ] = *sp;
		else
		  buffer[ i ] = ' ';

		sp++;
	}
  buffer[ length ] = 0;

	serial_print( buffer );



}

void busy( unsigned int num )
{
  unsigned int i, j;

  for ( i = 0; i < num; i++)
	  for( j = 1; j != 0; j++ );
}

