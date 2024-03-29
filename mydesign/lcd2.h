#ifndef __LCD_H__
#define __LCD_H__

/* Part 1:  68HC11 Definitions. */

#define M6811_BAUD	0x2B	/* SCI Baud register */
#define M6811_SCCR1	0x2C	/* SCI Control register 1 */
#define M6811_SCCR2	0x2D	/* SCI Control register 2 */
#define M6811_SCSR	0x2E	/* SCI Status register */
#define M6811_SCDR	0x2F	/* SCI Data (Read => RDR, Write => TDR) */

/* Flags of the SCCR2 register.  */
#define M6811_TE	0x08	/* Transmit Enable */
#define M6811_RE	0x04	/* Receive Enable */

/* Flags of the SCSR register.  */
#define M6811_TDRE	0x80	/* Transmit Data Register Empty */

/* Flags of the BAUD register.  */
#define M6811_SCP1	0x20	/* SCI Baud rate prescaler select */
#define M6811_SCP0	0x10
#define M6811_SCR2	0x04	/* SCI Baud rate select */
#define M6811_SCR1	0x02
#define M6811_SCR0	0x01

#define M6811_BAUD_DIV_1	(0)
#define M6811_BAUD_DIV_3	(M6811_SCP0)
#define M6811_BAUD_DIV_4	(M6811_SCP1)
#define M6811_BAUD_DIV_13	(M6811_SCP1|M6811_SCP0)

#define M6811_DEF_BAUD M6811_BAUD_DIV_4 /* 1200 baud */

/* The I/O registers are represented by a volatile array.
   Address if fixed at link time.  For this particular example,
   the _io_ports address is defined in the `memory.x' file.  */


/* Part 2:  68HC11 SIO Operations. */

/*! Send the character on the serial line.*/

static inline void serial_send (char c)
{
  /* Wait until the SIO has finished to send the character.  */
  while (!(_io_ports[M6811_SCSR] & M6811_TDRE))
    continue;

  _io_ports[M6811_SCDR] = c;
  _io_ports[M6811_SCCR2] |= M6811_TE;
}


void sys_print_lcd(char* text);
void _sys_init_lcd(void);

#define MAX_CHAR_COUNT 16
#define RESETV	(*(volatile int *)(0xBFC0 + 0x3E))
extern void _Main();

#endif 
