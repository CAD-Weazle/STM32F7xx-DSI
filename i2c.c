// file    : i2c.c
// author  : rb
// purpose : I2C routines for STM32F7xx
// date    : 251220
// last    : 251223
//

#include "includes.h"

// init I2C 
void init_i2c (void)
{
  // setup I2C module
//init_I2C4 ();
//init_i2c2 ();
//init_i2c3 ();
  init_i2c4 ();
}

// init I2C4 module
void init_i2c4 (void)
{
  // enable I2C4 clock (50Mc PCLK1)
  RCC->APB1ENR |= RCC_APB1ENR_I2C4EN;

  // disable I2C4 before changing settings
  I2C4->CR1 = 0;

  // setup bus timing - 100kHz clock 
  I2C4->TIMINGR =  (0x0b << 28)   // PRESC
                |  (0x04 << 20)   // SCLDEL
                |  (0x02 << 16)   // SDADEL
                |  (0x12 << 8)    // SCLH // hi: 4.6us
                |   0x15;         // SCLL // lo: 5.4us

  // enable I2C4 again, no filters, no interrupts, no DMA
  I2C4->CR1 |= I2C_CR1_PE;
  usleep (5);  // relax a bit as per UM
}

// I2C4 write byte
void i2c4_write_byte (uint8_t adr, uint8_t dat)
{
  // setup Tx mode: 7-bit, automatic end, no reload 
  I2C4->CR2 =  I2C_CR2_AUTOEND              // automatic end mode
            | (1 << I2C_CR2_NBYTES_Pos)     // number of bytes
          //|  I2C_CR2_RD_WRN               // 0: write / 1: read
            |  adr;                         // slave address

  // generate START condition
  I2C4->CR2 |= I2C_CR2_START;

  // wait TXDR empty
  while ((I2C4->ISR & I2C_ISR_TXIS) == 0)
  {
    // bail on NACK
    if ((I2C4->ISR & I2C_ISR_NACKF) != 0)
    {
      printf ("ERROR: NACK found, bail Tx\n");
      return;
    }
  }
    
  // write TXDR
  I2C4->TXDR = dat; 
}
  
// I2C4 read byte
uint8_t i2c4_read_byte (uint8_t adr)
{
  // setup Rx mode: 7-bit, automatic end, no reload 
  I2C4->CR2 =  I2C_CR2_AUTOEND              // automatic end mode
            | (1 << I2C_CR2_NBYTES_Pos)     // number of bytes
            |  I2C_CR2_RD_WRN               // 0: write / 1: read
            |  adr;                         // slave address

  // generate START condition
  I2C4->CR2 |= I2C_CR2_START;

  // wait RXDR not empty
  while ((I2C4->ISR & I2C_ISR_RXNE) == 0)
    ;
 
  // read TXDR
  return (I2C4->RXDR);
}

// ----- dev only -----

// scan I2C4 bus for active devices
void i2c4_scan_bus (void)
{
  for (int adr = 0x00; adr <= 0xff; adr++)
  {
    // enable I2C4 
    I2C4->CR1 |= I2C_CR1_PE;
    usleep (5);  // relax a bit as per UM

    printf ("slave address: 0x%02x: ", adr);
  
    // setup Tx mode: 7-bit, automatic end, no reload 
    I2C4->CR2 =  I2C_CR2_AUTOEND              // automatic end mode
              | (1 << I2C_CR2_NBYTES_Pos)     // number of bytes
              |  adr;                         // slave address

    // generate START event
    I2C4->CR2 |= I2C_CR2_START;

    // wait transfer ready
    msleep (50);

    // check for NACK
    if ((I2C4->ISR & I2C_ISR_NACKF) != 0)
    {
      printf ("NACK\n");
    }
    else
    { 
      printf ("address active\n");
    }

    // disable I2C4. clear all state
    I2C4->CR1 &=~I2C_CR1_PE;
  }
}

// dump I2C4 registers
void i2c4_dump_registers (void)
{
  printf ("I2C4_CR1      : 0x%08lx\n", I2C4->CR1);
  printf ("I2C4_CR2      : 0x%08lx\n", I2C4->CR2);
  printf ("I2C4_OAR1     : 0x%08lx\n", I2C4->OAR1);
  printf ("I2C4_OAR2     : 0x%08lx\n", I2C4->OAR2);
  printf ("I2C4_TIMINGR  : 0x%08lx\n", I2C4->TIMINGR);
  printf ("I2C4_TIMEOUTR : 0x%08lx\n", I2C4->TIMEOUTR);
  printf ("I2C4_ISR      : 0x%08lx\n", I2C4->ISR);
  printf ("I2C4_ICR      : 0x%08lx\n", I2C4->ICR);
  printf ("I2C4_PECR     : 0x%08lx\n", I2C4->PECR);
  printf ("I2C4_RXDR     : 0x%08lx\n", I2C4->RXDR);
  printf ("I2C4_TXDR     : 0x%08lx\n", I2C4->TXDR);
  printf ("\n");
}

