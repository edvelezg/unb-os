#ifndef __LCD_H__
#define __LCD_H__

void sys_print_lcd(char* text);
void _sys_init_lcd(void);

#define MAX_CHAR_COUNT 16
#define RESETV	(*(volatile int *)(0xBFC0 + 0x3E))
extern void _Main();

#endif
