// file    : main.c
// author  : rb
// purpose : STM32H769 DSI LCD bare metal
// board   : STM32H769I-DISCO Eval Board
// date    : 251203
// last    : 251219
//
// note    : STM32F769NIH6: 32-bit 2MB FLASH 512kB SRAM 216TFBGA

#include "includes.h"

int main (void)  
{
  // init all the things!1!!
  init_pll      ();          // enable clocks & PLL
  init_ports    ();          // setup GPIOs
  init_ticker   ();          // start timers
  init_uart6    (115200);    // init UART6 - debug port via Arduino connector CN13 pin D0/D1
  init_dsi      ();          // setup DSI
  init_otm8009a ();          // init OTM8009A LCD Display IC Driver

  while (1)
  {
    if (dsec_elapsed)
    {
      // draw filled rectangle
      fill_rect ();
  
      dsec_elapsed = 0;
    }

    if (sec_elapsed)
    {
      // der Blinkenlichten
      led_red_flash (10);
 
      sec_elapsed = 0;
    }
  }
}

