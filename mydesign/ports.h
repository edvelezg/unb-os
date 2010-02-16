#ifndef _PORTS_H_  
#define _PORTS_H_  

#define PORT_BASE 0x1000
 
#define TMSK1	0x22    /* OC1I:OC2I:OC3I:OC4I:OC5I:IC1I:IC2I:IC3I */
#define TFLG1	0x23    /* OC1F:OC2F:OC3F:OC4F:OC5F:IC1F:IC2F:IC3F */ 
#define TMSK2	0x24    /* TOI:RTII:PAOVI:PAII:0:0:PR1:PR0 */
#define TFLG2	0x25    /* TOF:RTIF:PAOVF:PAIF:0:0:0:0 */ 
#define TOC4    0x1C
#define TCNT	0x0E

/* Part 1:  68HC11 Definitions. */

#define M6811_BAUD      0x2B    /* SCI Baud register */
#define M6811_SCCR1     0x2C    /* SCI Control register 1 */
#define M6811_SCCR2     0x2D    /* SCI Control register 2 */
#define M6811_SCSR      0x2E    /* SCI Status register */
#define M6811_SCDR      0x2F    /* SCI Data (Read => RDR, Write => TDR) */

/* Flags of the SCCR2 register.  */
#define M6811_TE        0x08    /* Transmit Enable */
#define M6811_RE        0x04    /* Receive Enable */

/* Flags of the SCSR register.  */
#define M6811_TDRE      0x80    /* Transmit Data Register Empty */

/* Flags of the BAUD register.  */
#define M6811_SCP1      0x20    /* SCI Baud rate prescaler select */
#define M6811_SCP0      0x10
#define M6811_SCR2      0x04    /* SCI Baud rate select */
#define M6811_SCR1      0x02
#define M6811_SCR0      0x01

#define M6811_BAUD_DIV_1        (0)
#define M6811_BAUD_DIV_3        (M6811_SCP0)
#define M6811_BAUD_DIV_4        (M6811_SCP1)
#define M6811_BAUD_DIV_13       (M6811_SCP1|M6811_SCP0)

#define M6811_DEF_BAUD M6811_BAUD_DIV_4 /* 1200 baud */

/* The I/O registers are represented by a volatile array.
   Address if fixed at link time.  For this particular example,
   the _io_ports address is defined in the `memory.x' file.  */

#define _io_ports ((volatile unsigned char*)(PORT_BASE))

#define TICKS_IN_MS 2000

#endif
