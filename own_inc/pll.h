// file    : pll.h
// author  : rb
// purpose : header file for pll.c
// date    : 171120
// last    : 251204
//
// note    : use same clock divers for APB1 and APB2 bus so Timers run on same clock speed
//
// note    : PLLP: 0b00 = /2
//           PLLP: 0b01 = /4
//           PLLP: 0b10 = /6
//           PLLP: 0b11 = /8

#ifndef _PLL_H_
#define _PLL_H_

// -- defines
#define Mc                          (1000000UL)  // 1 Mc
                                                 
#define HSE                              (25UL)  // HSE clock - for calculations only, fix overflow errors
#define HSE_CLK                       (25UL*Mc)  // HSE clock = 25Mc - on STM32F769I-DISCO eval board
#define HSI_CLK                       (16UL*Mc)  // HSI clock = 16Mc

#define PLLM                             (25UL)  // PLL divider     - division factor for the main PLLs
#define PLLN                            (400UL)  // PLL multiplier  - main PLL multiplication factor for VCO
#define PLLP                              (0UL)  // PLL divider     - main PLL division factor for main system clock
#define PDIV                     ((PLLP+1) * 2)  // calculated dividing value from PLLP
#define PLLQ                              (9UL)  // PLL divider     - divider for USB/SDMMC/RND <> not used

#define PLL_CLK   (((HSE*PLLN)/(PLLM*PDIV))*Mc)  // PLL clock - 200Mc

#define SYSCLK                        (PLL_CLK)  // system clock - 200Mc
#define HCLK                      (PLL_CLK/1UL)  // system clock divided by AHB pre-scaler (here: /1)

#define APB1DIV                           (4UL)  // APB1 clock divider
#define PCLK1                    (HCLK/APB1DIV)  // APB1 Peripheral clock
#define TCLK1                ((2*HCLK)/APB1DIV)  // APB1 Timer clock (with clock doubler when APB1DIV > 1)

#define APB2DIV                           (4UL)  // APB2 clock divider
//#define APB2DIV                         (2UL)  // APB2 clock divider - HAL code
#define PCLK2                    (HCLK/APB2DIV)  // APB2 Peripheral clock
#define TCLK2                ((2*HCLK)/APB2DIV)  // APB2 Timer clock (with clock doubler when APB1DIV > 1)

//#define STRINGIFY2(x) #x
//#define STRINGIFY(x) STRINGIFY2(x)
//
//#pragma message ("HSE_CLK = " STRINGIFY(HSE_CLK))
//#pragma message ("PLLN    = " STRINGIFY(PLLN))
//#pragma message ("PLLM    = " STRINGIFY(PLLM))
//#pragma message ("PLLP    = " STRINGIFY(PLLP))
//#pragma message ("PDIV    = " STRINGIFY(PDIV))
//#pragma message ("PLL_CLK = " STRINGIFY(PLL_CLK))
//#pragma message ("APB1CLK = " STRINGIFY(APB1CLK))
//#pragma message ("APB2CLK = " STRINGIFY(APB2CLK))

// -- prototypes
void init_pll (void);

#endif
