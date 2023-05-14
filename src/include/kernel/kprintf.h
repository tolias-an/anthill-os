/*
* Small printf for kernel usage. Supports decimals, hexadecimals and strings
*/
#ifndef _K_PRINTF
#define _K_PRINTF

/*
* Send formatted string to UART
*/
void kprintf(char *format, ...);

#endif /* _K_PRINTF */
