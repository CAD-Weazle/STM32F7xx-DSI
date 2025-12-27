// file    : ports.c
// author  : ao/rb
// purpose : STM32F7xx IO port routines
// date    : 251203
// last    : 251203
//

#include "includes.h"

// GPIO bit fields
static const struct port_info
{
  uint16_t mode : 2;
  uint16_t speed: 2;
  uint16_t od   : 1;
  uint16_t pupd : 2;
  uint16_t afr  : 4;
} port_info_type; 

// GPIO Port A settings
static const struct port_info porta[16] = 
{
  [ 0] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLUP},                  // B_USER button
  [ 1] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //   
  [ 2] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //    
  [ 3] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //    
  [ 4] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //   
  [ 5] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //   
  [ 6] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //   
  [ 7] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //   
//[ 8] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //   
  [ 8] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF0_MCO},    // MCO
  [ 9] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //   
  [10] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //   
  [11] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //   
  [12] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //   
  [13] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF0_SWJ},    // SWD_IO   - prog port
  [14] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF0_SWJ},    // SWD_CLK  - prog port
  [15] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
};

// GPIO Port B settings
static const struct port_info portb[16] = 
{
  [ 0] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //  
  [ 1] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [ 2] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [ 3] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [ 4] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [ 5] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [ 6] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [ 7] = {GPIO_MODE_AF, GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_OD, GPIO_PULLUP,   GPIO_AF11_I2C4}, // I2C4_SDA - LCD_SDA
  [ 8] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [ 9] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [10] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_OD, GPIO_PULLDOWN},                 // 
  [11] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_OD, GPIO_PULLDOWN},                 // 
  [12] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [13] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [14] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [15] = {GPIO_MODE_IN, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
};

// GPIO Port C settings
static const struct port_info portc[16] = 
{
  [ 0] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 1] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 2] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 3] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 4] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 5] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 6] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL,  GPIO_AF8_USART6},// 
  [ 7] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL,  GPIO_AF8_USART6},// 
  [ 8] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 9] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [10] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [11] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [12] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  //
  [13] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [14] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [15] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
};


//gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
//gpio_init_structure.Pull      = GPIO_PULLUP;
//gpio_init_structure.Speed     = GPIO_SPEED_FAST;
//gpio_init_structure.Alternate = GPIO_AF12_FMC;


// GPIO Port D settings
static const struct port_info portd[16] = 
{
  [ 0] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D2
  [ 1] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D3
  [ 2] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //  
  [ 3] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 4] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 5] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 6] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 7] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 8] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D13
  [ 9] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D14 
  [10] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D15 
  [11] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [12] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_OD, GPIO_PULLUP, GPIO_AF4_I2C4},   // I2C4_SCL - LCD_SCL
  [13] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [14] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D0 
  [15] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D1 
};

// GPIO Port E settings
static const struct port_info porte[16] = 
{
  [ 0] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_NBL0
  [ 1] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_NBL1
  [ 2] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //  
  [ 3] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 4] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 5] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 6] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 7] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D4
  [ 8] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D5
  [ 9] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D6 
  [10] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D7 
  [11] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D8
  [12] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D9
  [13] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D10
  [14] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D11
  [15] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D12
};

// GPIO Port F settings
static const struct port_info portf[16] = 
{
  [ 0] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A0
  [ 1] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A1
  [ 2] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A2 
  [ 3] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A3
  [ 4] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A4
  [ 5] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A5
  [ 6] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 7] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 8] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 9] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [10] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [11] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_SDNRAS
  [12] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A6
  [13] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A7
  [14] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A8
  [15] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A9
};

// GPIO Port G settings
static const struct port_info portg[16] = 
{
  [ 0] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A10
  [ 1] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A11
  [ 2] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_A12
  [ 3] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [ 4] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_BA0
  [ 5] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_BA1
  [ 6] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [ 7] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [ 8] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_SDCLK
  [ 9] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [10] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [11] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [12] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [13] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [14] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [15] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_SDNCAS
};

// GPIO Port H settings
static const struct port_info porth[16] = 
{
//[ 0] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP},                  // OSC_IN
//[ 1] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP},                  // OSC_OUT
  [ 2] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_SDCKE0
  [ 3] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_SDNE0
  [ 4] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [ 5] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_SDNWE
  [ 6] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [ 7] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [ 8] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D16
  [ 9] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D17
  [10] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D18
  [11] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D19
  [12] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D20
  [13] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D21
  [14] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D22
  [15] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D23
};

// GPIO Port I settings
static const struct port_info porti[16] = 
{
  [ 0] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D24
  [ 1] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D25 
  [ 2] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D26 
  [ 3] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D27 
  [ 4] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_NBL2 
  [ 5] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_NBL3
  [ 6] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D28
  [ 7] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D29
  [ 8] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLUP},                  //
  [ 9] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D30
  [10] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF12_FMC},   // FMC_D31
  [11] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [12] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [13] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP},                  // TOUCH_INT
  [14] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // LCD_BL_CTRL 
  [15] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
};

// GPIO Port J settings
static const struct port_info portj[16] = 
{
  [ 0] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // 
  [ 1] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // 
  [ 2] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF13_DSI},   // DSI_TE   - tearing effect from TFT
  [ 3] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 4] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [ 5] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // LD_USER2
  [12] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // 
  [13] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // LD_USER1
  [14] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // 
  [15] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLUP},                  // DSI_RESET - for MB1188 TFT board!!!
};

// init GPIO Port
static void init_port (GPIO_TypeDef *GPIOx, const struct port_info *port)
{
  uint32_t mode = 0, speed = 0, type = 0, pupd = 0;
  uint64_t afr = 0;

  for (int i = 0; i < 16; i++)
  {
    mode  |= (uint32_t) port[i].mode  << i*2;
    speed |= (uint32_t) port[i].speed << i*2;
    pupd  |= (uint32_t) port[i].pupd  << i*2;
    type  |= (uint32_t) port[i].od    << i;
    afr   |= (uint64_t) port[i].afr   << i*4;
  }

  GPIOx->AFR[0]  = afr;           // set AFR registers first (no spurs on UART Tx)
  GPIOx->AFR[1]  = afr >> 32;     // set AFR registers first (no spurs on UART Tx)
  GPIOx->MODER   = mode;
  GPIOx->OSPEEDR = speed;
  GPIOx->PUPDR   = pupd;
  GPIOx->OTYPER  = type;
}

// initialize GPIO ports
void init_ports (void) 
{
  // enable clocks for GPIO ports
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN 
               |  RCC_AHB1ENR_GPIOBEN 
               |  RCC_AHB1ENR_GPIOCEN 
               |  RCC_AHB1ENR_GPIODEN 
               |  RCC_AHB1ENR_GPIOEEN 
               |  RCC_AHB1ENR_GPIOFEN 
               |  RCC_AHB1ENR_GPIOGEN 
               |  RCC_AHB1ENR_GPIOHEN 
               |  RCC_AHB1ENR_GPIOIEN 
               |  RCC_AHB1ENR_GPIOJEN;

  // setup GPIO ports
  init_port (GPIOA, porta);
  init_port (GPIOB, portb);
  init_port (GPIOC, portc);
  init_port (GPIOD, portd);
  init_port (GPIOE, porte);
  init_port (GPIOF, portf);
  init_port (GPIOG, portg);
  init_port (GPIOH, porth);
  init_port (GPIOI, porti);
  init_port (GPIOJ, portj);
  
  // LEDs off
  led_red_off ();
  led_grn_off ();

  // TFT LCD backlight on & reset add-on board
  tft_bl_on ();
  tft_reset ();
}


// red LED routines
void led_red_on (void)
{
  gpio_set (GPIOJ, LD_USER1);
}

void led_red_off (void)
{
  gpio_clr (GPIOJ, LD_USER1);
}

void led_red_toggle (void)
{
  if (GPIOJ->ODR & (1 << LD_USER1))
    gpio_clr (GPIOJ, LD_USER1);
  else
    gpio_set (GPIOJ, LD_USER1);
}

void led_red_flash (uint16_t delay)
{
  gpio_set (GPIOJ, LD_USER1);
  msleep (delay);
  gpio_clr (GPIOJ, LD_USER1);
}

// green LED routines
void led_grn_on (void)
{
  gpio_set (GPIOJ, LD_USER2);
}

void led_grn_off (void)
{
  gpio_clr (GPIOJ, LD_USER2);
}

void led_grn_toggle (void)
{
  if (GPIOJ->ODR & (1 << LD_USER2))
    gpio_clr (GPIOJ, LD_USER2);
  else
    gpio_set (GPIOJ, LD_USER2);
}

void led_grn_flash (uint16_t delay)
{
  gpio_set (GPIOJ, LD_USER2);
  msleep (delay);
  gpio_clr (GPIOJ, LD_USER2);
}

// TFT LCD backlight control
void tft_bl_on (void)
{
  gpio_set (GPIOI, LCD_BL_CTRL);
//printf ("BACKLIGHT ON\n");
}

// reset TFT LCD & Touch controller - RESET active low
void tft_reset (void)
{
  gpio_clr (GPIOJ, DSI_RESET); 
  msleep (10);
  gpio_set (GPIOJ, DSI_RESET); 
  msleep (10);
}
