// file    : ticker.c
// author  : rb
// purpose : STM32F7xx timer routines
// date    : 251205
// last    : 251204
//
// note    : Timer 6 usleep timer

#include "includes.h"

// globals
volatile int csec_elapsed = 0;
volatile int dsec_elapsed = 0;
volatile int  sec_elapsed = 0;

// Timer 7 overflow interrupt
void TIM7_IRQHandler (void)
{
  static int msec_cnt = 0;
  static int csec_cnt = 0;
  static int dsec_cnt = 0;

  // clear interrupt (note: first thing to do in the ISR!!!!!)
  TIM7->SR = 0;
 
  msec_cnt++;

  if (msec_cnt >= 10)
  {
    csec_elapsed = 1;

    msec_cnt = 0;
    csec_cnt++;
  }

  if (csec_cnt == 10)
  {
    dsec_elapsed = 1;

    csec_cnt = 0;
    dsec_cnt++;
  }

  if (dsec_cnt == 10)
  {
    sec_elapsed = 1;
    dsec_cnt = 0;
  }
}        

// setup Timers
void init_ticker (void)
{
  // enable clock for Timers
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN 
               |  RCC_APB1ENR_TIM7EN;
   
  // init 16-bit Timer 6, freerunning at 1 MHz - 'usleep ()' 
  TIM6->PSC    = TIMER_1MHZ;                // timer_clock
  TIM6->EGR    = TIM_EGR_UG;                // force update
  TIM6->CR1    = TIM_CR1_CEN;               // enable timer

  // init 16-bit Timer 7, generate interrupt every ms - wall clock
  TIM7->PSC    = TIMER_1MHZ;                // timer_clock
  TIM7->ARR    = 2000;                      // set period <> should be 1000? (??)
  TIM7->EGR    = TIM_EGR_UG;                // force update
  TIM7->DIER   = TIM_DIER_UIE;              // enable interrupts, disable DMA
  TIM7->CR1    = TIM_CR1_CEN;               // enable timer
  NVIC_EnableIRQ (TIM7_IRQn);               // enable interrupt Timer 7
}

// kill time with 1 us resolution
void usleep (int16_t usecs)
{
  uint16_t start = TIM6->CNT + usecs;

  while ((int16_t)(TIM6->CNT - start) < 0)
    continue;
}

// kill time with ms resolution
void msleep (uint16_t msecs)
{
  while (msecs--)
    usleep (1000); 
}

// dump Timer registers -- dev only
void timer_dump_registers (void)
{
//printf ("-- Timer 7 registers --\n");
//printf ("TIM7->PSC  : %08lx\n", TIM7->PSC);
//printf ("TIM7->ARR  : %08lx\n", TIM7->ARR);
//printf ("TIM7->EGR  : %08lx\n", TIM7->EGR);
//printf ("TIM7->CR1  : %08lx\n", TIM7->CR1);
//printf ("TIM7->CR2  : %08lx\n", TIM7->CR2);  
//printf ("TIM7->SMCR : %08lx\n", TIM7->SMCR);  
//printf ("TIM7->CCMR1: %08lx\n", TIM7->CCMR1);  
//printf ("TIM7->CCMR2: %08lx\n", TIM7->CCMR2);
//printf ("TIM7->CCER : %08lx\n", TIM7->CCER);	
}
