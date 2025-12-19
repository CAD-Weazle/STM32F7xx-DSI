// file    : uart6.h
// author  : rb
// purpose : header file for uart6.c
// date    : 170813
// last    : 251204
//

#ifndef _UART6_H_
#define _UART6_H_

// -- defines
#define RX6_SIZE             128
#define TX6_SIZE             128

// -- prototypes
void init_uart6 (int baud);

void uart6_putc (int c);
void uart6_puts (const char *s);
int  uart6_getc (void);

int uart6_flush (void *dev);

#endif