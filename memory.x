/* memory.x -- Memory definition for a bootstrap program
   Copyright 2000, 2003 Free Software Foundation, Inc.
   Written by Stephane Carrez (stcarrez@nerim.fr)

 */

/* Defines the memory layout for a bootstrap program.
   Pretend there is no data section.  Everything is for the text.
   The page0 overlaps the text and we have to take care of that
   in the program (this is volunteered).  */
MEMORY
{
  page0 (rwx) : ORIGIN = 0x0f0, LENGTH = 15
  text  (rx)  : ORIGIN = 0x0, LENGTH = 0x0f0
  data        : ORIGIN = 0x0, LENGTH = 0
}

/* Setup the stack on the top of the data internal ram (not used).  */
PROVIDE (_stack = 0x0100-1);
PROVIDE (_io_ports = 0x1000);
