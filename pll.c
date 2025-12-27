// file    : pll.c
// author  : rb
// purpose : STM32F7xx RCC module routines 
// date    : 251203
// last    : 251204
//
// note    : max. SYSCLK  = 216Mc
//           max. APB1CLK =  54Mc
//           max. APB2CLK = 108Mc
//
// note    : keep APB1CLK and APB2CLK at same frequency for simplicity for now (only change when really needed)

#include "includes.h"

// setup clocks
void init_pll (void) 
{
  // set MCO1 and bus dividers
  RCC->CFGR = RCC_CFGR_MCO1PRE_2   |        // 0b110: MCO1 divided by 4
              RCC_CFGR_MCO1PRE_1   |
            //RCC_CFGR_MCO1PRE_0   |
              RCC_CFGR_MCO1_1      |        // 0b11: PLL on MCO1 / 0b10: HSE on MCO1
              RCC_CFGR_MCO1_0      |
              RCC_CFGR_HPRE_DIV1   |        // AHB  = 200 MHz
              RCC_CFGR_PPRE1_DIV4  |        // APB1 =  50 MHz			  
              RCC_CFGR_PPRE2_DIV4;          // APB2 =  50 MHz			  
			  
  // enable power interface
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  
  // enable over-drive mode (needed when >180Mc) 
  PWR->CR1 |= PWR_CR1_ODEN;

  // wait over-drive mode ready
  while (!(PWR->CSR1 & PWR_CSR1_ODRDY))
    ;  

  // switch over-drive mode
  PWR->CR1 |= PWR_CR1_ODSWEN;
  
  // wait over-drive mode switch ready
  while (!(PWR->CSR1 & PWR_CSR1_ODSWRDY))
    ; 

//// enable HSI - not used now
//RCC->CR |= RCC_CR_HSION;
//
//// wait for HSI stable
//while ((RCC->CR & RCC_CR_HSIRDY) == 0)
//  ;                   

  // enable HSE - used as clock source of PLL
  RCC->CR |= RCC_CR_HSEON;
  
  // wait for HSE stable
  while ((RCC->CR & RCC_CR_HSERDY) == 0)
    ;                   

  // configure PLL at 200Mc
  RCC->PLLCFGR = (PLLM << RCC_PLLCFGR_PLLM_Pos) | 
                 (PLLN << RCC_PLLCFGR_PLLN_Pos) |
                 (PLLP << RCC_PLLCFGR_PLLP_Pos) |
                 (PLLQ << RCC_PLLCFGR_PLLQ_Pos) |  // for USB, wrong clock freq now, fix if needed
                  RCC_PLLCFGR_PLLSRC_HSE;
  
  // enable PLL 
  RCC->CR |= RCC_CR_PLLON;
  
  // wait PLL ready
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;     
  
  // set FLASH latency - for 200Mc requires 7 wait states
  FLASH->ACR = FLASH_ACR_LATENCY_7WS | FLASH_ACR_PRFTEN;
  
  // select PLL as system clock SYSCLK
  RCC->CFGR &= ~RCC_CFGR_SW;     // clear SW[1:0] bits
  RCC->CFGR |= RCC_CFGR_SW_PLL;  // select PLL
  
  // wait switch ready
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ; 
}

// dump defines from 'pll.h'
void dump_clock_defs (void)
{
  printf ("PLLM       %ld\n", PLLM);
  printf ("PLLN       %ld\n", PLLN);
  printf ("PLLP       %ld -> ", PLLP);
  printf ("PDIV = %ld\n", PDIV);
  printf ("PLL_CLK    %ld\n", PLL_CLK);
  printf ("SYSCLK     %ld\n", SYSCLK);
  printf ("HCLK       %ld\n", HCLK);
  printf ("APB1DIV    %ld\n", APB1DIV);
  printf ("PCLK1      %ld\n", PCLK1);
  printf ("TCLK1      %ld\n", TCLK1);
  printf ("APB2DIV    %ld\n", APB2DIV);
  printf ("PCLK2      %ld\n", PCLK2);
  printf ("TCLK2      %ld\n", TCLK2);
  printf ("----------\n");
  printf ("TIMER_1MHZ %ld\n", TIMER_1MHZ);
  printf ("----------\n");
}
