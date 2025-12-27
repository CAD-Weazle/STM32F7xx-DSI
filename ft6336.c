// file    : ft6336.c
// author  : rb
// purpose : I2C routines for FT6336 Self-Capacitive Touch Panel Controller
// date    : 251223
// last    : 251223
//

#include "includes.h"

// handle new touch data FT6336 - runtime: 3.6ms
void EXTI15_10_IRQHandler (void)
{
  // clear pending interrupt EXTI13 (PI[13] - LCD_INT)
  EXTI->PR = EXTI_PR_PR13;            

  // for timing <> dev only
  led_grn_on ();

  // get point 1 coordinates
  ts.x1_pos = ft6336_read_reg16 (FT6336_REG_TOUCH1_XH) & 0x0fff;
  ts.y1_pos = ft6336_read_reg16 (FT6336_REG_TOUCH1_YH) & 0x0fff;

  // get point 2 coordinates
  ts.x2_pos = ft6336_read_reg16 (FT6336_REG_TOUCH2_XH) & 0x0fff;
  ts.y2_pos = ft6336_read_reg16 (FT6336_REG_TOUCH2_YH) & 0x0fff;

  // get number of touch points & set flag
  ts.num_points = ft6336_read_reg8 (FT6336_REG_TD_STATUS) & 0x0f;
  ts.new_points = 1;

  // for timing <> dev only
  led_grn_off ();
}


// init I2C 
void init_ft6336 (void)
{
  // enable SYSCFG block
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  // set EXTI event for LCD_INT touch detection on PI[13]
  SYSCFG->EXTICR[3] = SYSCFG_EXTICR4_EXTI13_PI;  // select PI[13] as source for EXTI event - LCD_INT

  // setup EXTI interrupt/event controller
  EXTI->PR   =  EXTI_PR_PR13;                    // clear interrupt pending bit EXTI line 13
  EXTI->IMR |=  EXTI_IMR_MR13;                   // enable interrupt on EXTI line 13
  EXTI->EMR &= ~EXTI_EMR_MR13;                   // disable event on EXTI line 13

  // catch falling edge on LCD_INT
//EXTI->RTSR |=  EXTI_RTSR_TR13;                 // enable trigger rising edge on EXTI line 3
  EXTI->RTSR &= ~EXTI_RTSR_TR13;                 // disable trigger rising edge on EXTI line 3
  EXTI->FTSR |=  EXTI_FTSR_TR13;                 // enable trigger falling edge on EXTI line 3
//EXTI->FTSR &= ~EXTI_FTSR_TR13;                 // disable trigger falling edge on EXTI line 3

  // enable EXTI Interrupt in NVIC - Touch Controller ISR
   NVIC_SetPriority (EXTI15_10_IRQn, 3); 
   NVIC_EnableIRQ   (EXTI15_10_IRQn);
}

// get current touch panel state - polling mode 
void ft6336_update (void)
{
  // no touch detected, bail
  if ((GPIOI->IDR & (1 << LCD_INT)) != 0)
    return;

  // get point 1 coordinates
  ts.x1_pos = ft6336_read_reg16 (FT6336_REG_TOUCH1_XH) & 0x0fff;
  ts.y1_pos = ft6336_read_reg16 (FT6336_REG_TOUCH1_YH) & 0x0fff;

  // get point 2 coordinates
  ts.x2_pos = ft6336_read_reg16 (FT6336_REG_TOUCH2_XH) & 0x0fff;
  ts.y2_pos = ft6336_read_reg16 (FT6336_REG_TOUCH2_YH) & 0x0fff;

  // get number of touch points & set flag
  ts.num_points = ft6336_read_reg8 (FT6336_REG_TD_STATUS) & 0x0f;
  ts.new_points = 1;
}

// read 8-bit register
uint8_t ft6336_read_reg8 (uint8_t reg)
{
  // write register number
  i2c4_write_byte (FT6336_I2C_ADR, reg);  

  // read register data
  return (i2c4_read_byte (FT6336_I2C_ADR));
}

// read 16-bit register
uint16_t ft6336_read_reg16 (uint8_t reg)
{
  uint8_t hi, lo;

  // get hi-part
  i2c4_write_byte (FT6336_I2C_ADR, reg);  
  hi = i2c4_read_byte (FT6336_I2C_ADR);

  // get lo-part
  i2c4_write_byte (FT6336_I2C_ADR, reg+1);  
  lo = i2c4_read_byte (FT6336_I2C_ADR);

  return (((uint16_t)hi << 8) | lo);
}

// dump touch state
void ft6336_dump (void)
{
  // no new touch data, bail
  if (ts.new_points == 0)
    return;

  // reset flag
  ts.new_points = 0;

  // print touch data
  printf ("num: %d ", ts.num_points);
  printf ("- ");
  printf ("P1: (%d, %d) ", ts.x1_pos, ts.y1_pos);
  printf ("P2: (%d, %d) ", ts.x2_pos, ts.y2_pos);
  printf ("\n");
}

// dump FT6336 ID & firmware version
void ft6336_id (void)
{
  // seems not implemented, always reads back 0x00
  printf ("lib version    : %d\n", ft6336_read_reg8 (FT6336_REG_LIB_VERSION));
  printf ("chip ID/cypher : %d\n", ft6336_read_reg8 (FT6336_REG_CIPHER));
  printf ("interrupt mode : %d\n", ft6336_read_reg8 (FT6336_REG_GMODE));
  printf ("firmware ID    : %d\n", ft6336_read_reg8 (FT6336_REG_FIRMID));
  printf ("vendor ID      : %d\n", ft6336_read_reg8 (FT6336_REG_VENDORID));
  printf ("FT5335 state   : %d\n", ft6336_read_reg8 (FT6336_REG_STATE));
  printf ("threashold     : %d\n", ft6336_read_reg8 (FT6336_REG_TH_GROUP));
  printf ("rate active    : %d\n", ft6336_read_reg8 (FT6336_REG_PERIODACTIVE)); 
  printf ("rate monitor   : %d\n", ft6336_read_reg8 (FT6336_REG_PERIODMONITOR));
}









