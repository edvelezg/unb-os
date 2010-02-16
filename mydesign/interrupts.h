#ifndef _INTERRUPTS_H_  
#define _INTERRUPTS_H_  

typedef void (*interrupt_t) (void);

#define VECTOR_BASE     0xFFC0

#define TOC5V   *(interrupt_t *)(VECTOR_BASE + 0x20) /* TOC 5 */
#define TOC4V   *(interrupt_t *)(VECTOR_BASE + 0x22) /* TOC 4 */
#define TOC3V   *(interrupt_t *)(VECTOR_BASE + 0x24) /* TOC 3 */
#define TOC2V   *(interrupt_t *)(VECTOR_BASE + 0x26) /* TOC 2 */
#define TOC1V   *(interrupt_t *)(VECTOR_BASE + 0x28) /* TOC 1 */
#define IC3V    *(interrupt_t *)(VECTOR_BASE + 0x2A) /* IC3 */
#define IC2V    *(interrupt_t *)(VECTOR_BASE + 0x2C) /* IC2 */
#define IC1V    *(interrupt_t *)(VECTOR_BASE + 0x2E) /* IC1 */
#define RTIV    *(interrupt_t *)(VECTOR_BASE + 0x30) /* RTI */
#define SWIV    *(interrupt_t *)(VECTOR_BASE + 0x36) /* SWI */
#define RESETV  *(interrupt_t *) (VECTOR_BASE + 0x3E))

/* source: http://www.catonmat.net/blog/bit-hacks-header-file/ */
/* set n-th bit in x */
#define B_SET(x, n)      ((x) |= (1<<(n)))
/* unset n-th bit in x */
#define B_UNSET(x, n)    ((x) &= ~(1<<(n)))
/* toggle n-th bit in x */
#define B_TOGGLE(x, n)   ((x) ^= (1<<(n)))

#define storeSP(sp) asm("sts %0" : : "m"(sp))
#define loadSP(sp) asm("lds %0" : : "m"(sp))

void sys_print_lcd(char* text);
void _sys_init_lcd(void);

#endif /* _INTERRUPTS_H_ */  

