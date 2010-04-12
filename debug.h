#ifndef _DEBUG_H_  
#define _DEBUG_H_  


#define MAX_INT_LENGTH 16           // string length of a converted int to string
#define MAX_STRING_LENGTH 128

#define STACK_FUNC_OFFSET 0xa

unsigned int db_lock;

extern volatile unsigned char _io_ports[];

void serial_send( char c );

void serial_print( char *msg );
void serial_print_hex( unsigned int hex );

int mem_set_byte( unsigned int addr, unsigned char value );
int mem_set_int( unsigned int addr, unsigned int value );

unsigned char mem_get_byte( unsigned int addr );
unsigned int  mem_get_int( unsigned int addr );

void print_var( char* name, void* mem );
void mem_dump( unsigned int mem_start, unsigned int mem_end );
void mem_dump_old( unsigned int mem_start, unsigned int mem_end );
void stack_dump( unsigned int length );

void busy( unsigned int num );

#endif
