// file    : uart6.c
// author  : ao/rb
// purpose : STM32F7xx USART6 routines
// date    : 170811
// last    : 251204
//

#include "includes.h"

// input/output FIFOs 
//static char uart6_rx_buf[RX6_SIZE] __attribute__ ((section(".bss.ccm")));
//static char uart6_tx_buf[TX6_SIZE] __attribute__ ((section(".bss.ccm")));

static char uart6_rx_buf[RX6_SIZE];
static char uart6_tx_buf[TX6_SIZE];

// FIFO indexes 
volatile static uint32_t tx_tail = 0;
volatile static uint32_t rx_head = 0;
         static uint32_t tx_head = 0;
         static uint32_t rx_tail = 0;

// USART6 ISR
void USART6_IRQHandler (void)
{
  int sr = USART6->ISR;

  // read character
  if (sr & USART_ISR_RXNE)
  {
    int c = USART6->RDR;

    if ((int)(rx_head - rx_tail) < RX6_SIZE) 
      uart6_rx_buf[rx_head++ % RX6_SIZE] = c;
  }

  // write character
  if (sr & USART_ISR_TXE)
  {
    if (tx_head == tx_tail)
      USART6->CR1 &= ~USART_CR1_TXEIE; 
    else
      USART6->TDR = uart6_tx_buf[tx_tail++ % TX6_SIZE];
  }
}

// init USART6
void init_uart6 (int baud)
{
  // enable USART6 clock
  RCC->APB2ENR |= RCC_APB2ENR_USART6EN;

  USART6->CR3 = 0;
  USART6->CR2 = 0;                             // 1 stop bit
  USART6->BRR = ((PCLK2 << 4) / baud) >> 4;    // fixed-point that shit

  // note: oversampling by 16                    
  USART6->CR1 = USART_CR1_TE     |               
                USART_CR1_RE     |               
                USART_CR1_RXNEIE |               
                USART_CR1_UE;                    
/*
  USART6->BRR = ((APB1CLK << 4) / baud) >> 3;  // fixed-point that shit (note: OVER8 = 1, so '>> 3')
  USART6->CR1 = USART_CR1_OVER8  |             // 8x oversampling, for low 1.024Mc SYSCLK
                USART_CR1_TE     | 
                USART_CR1_RE     | 
                USART_CR1_RXNEIE | 
                USART_CR1_UE;
*/
  NVIC_EnableIRQ (USART6_IRQn);
}

// write character to USART6
void uart6_putc (int c)
{
  if (c == '\n')
    uart6_putc ('\r');

  while ((int)(tx_head - tx_tail) >= TX6_SIZE)
    ;

  uart6_tx_buf[tx_head++ % TX6_SIZE] = c;

  USART6->CR1 |= USART_CR1_TXEIE;
}

int uart6_flush (void *dev)
{
  while (tx_head != tx_tail)
    ;

  return 0;
}

// write string to USART6
void uart6_puts (const char *s)
{
  int c;

  while ((c = *s++) != 0)
    uart6_putc (c);
}

// get character from USART6
int uart6_getc (void)
{
  if (rx_head == rx_tail)
    return(-1);
  else
    return (uart6_rx_buf[rx_tail++ % RX6_SIZE]);
}

