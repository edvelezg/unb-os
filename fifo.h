#ifndef _FIFO_H_
#define _FIFO_H_

#include "os.h"
#include <stdlib.h>

typedef struct fifo_struct
{
    int     buffer[FIFOSIZE];  /* message circular buffer */
    int     fillCount;         /* keeps track of the number of items */          
    int     next;              /* next spot to write in */
    int     first;             /* first element to read */

} fifo_struct;

fifo_struct arrFifos[MAXFIFO];
static int numFifos = 0; /* keeps track of the number of fifos */

#endif /* _FIFO_H_ */
