// file    : otm8009a.c
// author  : st/rb
// purpose : Display Controller OTM8009A initialization (KoD LCD IC Driver)
// board   : STM32H769I-DISCO Eval Board
// date    : 251203
// last    : 251225
//
// note    : this code is taken from an ST HAL example
// note    : byte order is rearranged (last byte moved to position 1) for easier parsing

#include "includes.h"

// constant tables of register settings used to transmit DSI
// command packets as power up initialization sequence of the KoD LCD (OTM8009A LCD Driver)

// long packets - len | CMD | payload
const uint8_t LongRegData1[]  = { 4, 0xff, 0x80,0x09,0x01}; 
const uint8_t LongRegData2[]  = { 3, 0xff, 0x80,0x09};
const uint8_t LongRegData3[]  = {17, 0xe1, 0x00,0x09,0x0f,0x0e,0x07,0x10,0x0b,0x0a,0x04,0x07,0x0b,0x08,0x0f,0x10,0x0a,0x01};
const uint8_t LongRegData4[]  = {17, 0xe2, 0x00,0x09,0x0f,0x0e,0x07,0x10,0x0b,0x0a,0x04,0x07,0x0b,0x08,0x0f,0x10,0x0a,0x01};
const uint8_t LongRegData5[]  = { 3, 0xd8, 0x79,0x79};
const uint8_t LongRegData6[]  = { 3, 0xb3, 0x00,0x01};
const uint8_t LongRegData7[]  = { 7, 0xce, 0x85,0x01,0x00,0x84,0x01,0x00};
const uint8_t LongRegData8[]  = {15, 0xce, 0x18,0x04,0x03,0x39,0x00,0x00,0x00,0x18,0x03,0x03,0x3a,0x00,0x00,0x00};
const uint8_t LongRegData9[]  = {15, 0xce, 0x18,0x02,0x03,0x3b,0x00,0x00,0x00,0x18,0x01,0x03,0x3c,0x00,0x00,0x00};
const uint8_t LongRegData10[] = {11, 0xcf, 0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x02,0x00,0x00};
const uint8_t LongRegData11[] = {11, 0xcb, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t LongRegData12[] = {16, 0xcb, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t LongRegData13[] = {16, 0xcb, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t LongRegData14[] = {11, 0xcb, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t LongRegData15[] = {16, 0xcb, 0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t LongRegData16[] = {16, 0xcb, 0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00};
const uint8_t LongRegData17[] = {11, 0xcb, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t LongRegData18[] = {11, 0xcb, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
const uint8_t LongRegData19[] = {11, 0xcc, 0x00,0x26,0x09,0x0b,0x01,0x25,0x00,0x00,0x00,0x00};
const uint8_t LongRegData20[] = {16, 0xcc, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x0a,0x0c,0x02};
const uint8_t LongRegData21[] = {16, 0xcc, 0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t LongRegData22[] = {11, 0xcc, 0x00,0x25,0x0c,0x0a,0x02,0x26,0x00,0x00,0x00,0x00};
const uint8_t LongRegData23[] = {16, 0xcc, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x0b,0x09,0x01};
const uint8_t LongRegData24[] = {16, 0xcc, 0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t LongRegData25[] = {16, 0xcc, 0xff,0xff,0xff,0xff};    

// CASET value (Column Address Set) : X direction LCD GRAM boundaries
// depending on LCD orientation mode and PASET value (Page Address Set) : Y direction
// LCD GRAM boundaries depending on LCD orientation mode
// XS[15:0] = 0x000 = 0, XE[15:0] = 0x31F = 799 for landscape mode : apply to CASET
// YS[15:0] = 0x000 = 0, YE[15:0] = 0x31F = 799 for portrait mode : : apply to PASET
const uint8_t LongRegData27[] = { 5, OTM8009A_CMD_CASET, 0x00, 0x00, 0x03, 0x1f};

// XS[15:0] = 0x000 = 0, XE[15:0] = 0x1DF = 479 for portrait mode : apply to CASET
// YS[15:0] = 0x000 = 0, YE[15:0] = 0x1DF = 479 for landscape mode: apply to PASET
const uint8_t LongRegData28[] = { 5, OTM8009A_CMD_PASET, 0x00, 0x00, 0x01, 0xdf};

// short packets - CMD | payload
const uint8_t ShortRegData1[]  = {OTM8009A_CMD_NOP, 0x00};
const uint8_t ShortRegData2[]  = {OTM8009A_CMD_NOP, 0x80};
const uint8_t ShortRegData3[]  = {0xC4, 0x30};
const uint8_t ShortRegData4[]  = {OTM8009A_CMD_NOP, 0x8A};
const uint8_t ShortRegData5[]  = {0xC4, 0x40};
const uint8_t ShortRegData6[]  = {OTM8009A_CMD_NOP, 0xB1};
const uint8_t ShortRegData7[]  = {0xC5, 0xA9};
const uint8_t ShortRegData8[]  = {OTM8009A_CMD_NOP, 0x91};
const uint8_t ShortRegData9[]  = {0xC5, 0x34};
const uint8_t ShortRegData10[] = {OTM8009A_CMD_NOP, 0xB4};
const uint8_t ShortRegData11[] = {0xC0, 0x50};
const uint8_t ShortRegData12[] = {0xD9, 0x4E};
const uint8_t ShortRegData13[] = {OTM8009A_CMD_NOP, 0x81};
const uint8_t ShortRegData14[] = {0xC1, 0x66};
const uint8_t ShortRegData15[] = {OTM8009A_CMD_NOP, 0xA1};
const uint8_t ShortRegData16[] = {0xC1, 0x08};
const uint8_t ShortRegData17[] = {OTM8009A_CMD_NOP, 0x92};
const uint8_t ShortRegData18[] = {0xC5, 0x01};
const uint8_t ShortRegData19[] = {OTM8009A_CMD_NOP, 0x95};
const uint8_t ShortRegData20[] = {OTM8009A_CMD_NOP, 0x94};
const uint8_t ShortRegData21[] = {0xC5, 0x33};
const uint8_t ShortRegData22[] = {OTM8009A_CMD_NOP, 0xA3};
const uint8_t ShortRegData23[] = {0xC0, 0x1B};
const uint8_t ShortRegData24[] = {OTM8009A_CMD_NOP, 0x82};
const uint8_t ShortRegData25[] = {0xC5, 0x83};
const uint8_t ShortRegData26[] = {0xC4, 0x83};
const uint8_t ShortRegData27[] = {0xC1, 0x0E};
const uint8_t ShortRegData28[] = {OTM8009A_CMD_NOP, 0xA6};
const uint8_t ShortRegData29[] = {OTM8009A_CMD_NOP, 0xA0};
const uint8_t ShortRegData30[] = {OTM8009A_CMD_NOP, 0xB0};
const uint8_t ShortRegData31[] = {OTM8009A_CMD_NOP, 0xC0};
const uint8_t ShortRegData32[] = {OTM8009A_CMD_NOP, 0xD0};
const uint8_t ShortRegData33[] = {OTM8009A_CMD_NOP, 0x90};
const uint8_t ShortRegData34[] = {OTM8009A_CMD_NOP, 0xE0};
const uint8_t ShortRegData35[] = {OTM8009A_CMD_NOP, 0xF0};
const uint8_t ShortRegData36[] = {OTM8009A_CMD_SLPOUT, 0x00};
const uint8_t ShortRegData37[] = {OTM8009A_CMD_COLMOD, OTM8009A_COLMOD_RGB565};
const uint8_t ShortRegData38[] = {OTM8009A_CMD_COLMOD, OTM8009A_COLMOD_RGB888};
const uint8_t ShortRegData39[] = {OTM8009A_CMD_MADCTR, OTM8009A_MADCTR_MODE_LANDSCAPE};
const uint8_t ShortRegData40[] = {OTM8009A_CMD_WRDISBV, 0x7F};
const uint8_t ShortRegData41[] = {OTM8009A_CMD_WRCTRLD, 0x2C};
const uint8_t ShortRegData42[] = {OTM8009A_CMD_WRCABC, 0x02};
const uint8_t ShortRegData43[] = {OTM8009A_CMD_WRCABCMB, 0xFF};
const uint8_t ShortRegData44[] = {OTM8009A_CMD_DISPON, 0x00};
const uint8_t ShortRegData45[] = {OTM8009A_CMD_RAMWR, 0x00};
const uint8_t ShortRegData46[] = {0xCF, 0x00};
const uint8_t ShortRegData47[] = {0xC5, 0x66};
const uint8_t ShortRegData48[] = {OTM8009A_CMD_NOP, 0xB6};
const uint8_t ShortRegData49[] = {0xF5, 0x06};
const uint8_t ShortRegData50[] = {OTM8009A_CMD_NOP, 0xB1};
const uint8_t ShortRegData51[] = {0xC6, 0x05};
const uint8_t ShortRegData52[] = {OTM8009A_CMD_TEEON, 0x00};  // enable tearing effect output signal - not in ST example


// initialize OTM8009A MIPI DSI LCD driver
void init_otm8009a (void)
{
  // enable CMD2 to access vendor specific commands                               
  // enter in command 2 mode and set EXTC to enable address shift function (0x00) 
  dsi_shortwrite (ShortRegData1);
  dsi_longwrite (LongRegData1);

  // enter ORISE Command 2 
  dsi_shortwrite (ShortRegData2);
  dsi_longwrite (LongRegData2);

  // SD_PCH_CTRL - 0xC480h - 129th parameter - Default 0x00          
  // set SD_PT                                                       
  // -> source output level during porch and non-display area to GND 
  dsi_shortwrite (ShortRegData2);
  dsi_shortwrite (ShortRegData3);

  msleep (10);

  // not documented 
  dsi_shortwrite (ShortRegData4);
  dsi_shortwrite (ShortRegData5);

  msleep (10);

  // PWR_CTRL4 - 0xC4B0h - 178th parameter - Default 0xA8 
  // set gvdd_en_test                                     
  // -> enable GVDD test mode !!!                         
  dsi_shortwrite (ShortRegData6);
  dsi_shortwrite (ShortRegData7);

  // PWR_CTRL2 - 0xC590h - 146th parameter - Default 0x79      
  // set pump 4 vgh voltage                                    
  // -> from 15.0v down to 13.0v                               
  // set pump 5 vgh voltage                                    
  // -> from -12.0v downto -9.0v                               
  dsi_shortwrite (ShortRegData8);
  dsi_shortwrite (ShortRegData9);

  // P_DRV_M - 0xC0B4h - 181th parameter - Default 0x00 
  // -> column inversion                                
  dsi_shortwrite (ShortRegData10);
  dsi_shortwrite (ShortRegData11);

  // VCOMDC - 0xD900h - 1st parameter - Default 0x39h 
  // VCOM Voltage settings                            
  // -> from -1.0000v downto -1.2625v                 
  dsi_shortwrite (ShortRegData1);
  dsi_shortwrite (ShortRegData12);

  // oscillator adjustment for Idle/Normal mode (LPDT only) set to 65Hz (default is 60Hz) 
  dsi_shortwrite (ShortRegData13);
  dsi_shortwrite (ShortRegData14);

  // video mode internal 
  dsi_shortwrite (ShortRegData15);
  dsi_shortwrite (ShortRegData16);

  // PWR_CTRL2 - 0xC590h - 147h parameter - Default 0x00 
  // set pump 4&5 x6                                     
  // -> ONLY VALID when PUMP4_EN_ASDM_HV = "0"           
  dsi_shortwrite (ShortRegData17);
  dsi_shortwrite (ShortRegData18);

  // PWR_CTRL2 - 0xC590h - 150th parameter - Default 0x33h 
  // Change pump4 clock ratio                              
  // -> from 1 line to 1/2 line                            
  dsi_shortwrite (ShortRegData19);
  dsi_shortwrite (ShortRegData9);

  // GVDD/NGVDD settings 
  dsi_shortwrite (ShortRegData1);
  dsi_longwrite (LongRegData5);

  // PWR_CTRL2 - 0xC590h - 149th parameter - Default 0x33h 
  // rewrite the default value !                           
  dsi_shortwrite (ShortRegData20);
  dsi_shortwrite (ShortRegData21);

  // panel display timing Setting 3 
  dsi_shortwrite (ShortRegData22);
  dsi_shortwrite (ShortRegData23);

  // power control 1 
  dsi_shortwrite (ShortRegData24);
  dsi_shortwrite (ShortRegData25);

  // source driver precharge 
  dsi_shortwrite (ShortRegData13);
  dsi_shortwrite (ShortRegData26);
  dsi_shortwrite (ShortRegData15);
  dsi_shortwrite (ShortRegData27);
  dsi_shortwrite (ShortRegData28);
  dsi_longwrite (LongRegData6);

  // GOAVST 
  dsi_shortwrite (ShortRegData28);
  dsi_longwrite (LongRegData7);

  dsi_shortwrite (ShortRegData29);
  dsi_longwrite (LongRegData8);

  dsi_shortwrite (ShortRegData30);
  dsi_longwrite (LongRegData8);

  dsi_shortwrite (ShortRegData31);
  dsi_longwrite (LongRegData10);

  dsi_shortwrite (ShortRegData33);
  dsi_shortwrite (ShortRegData46);
  dsi_shortwrite (ShortRegData2);
  dsi_longwrite (LongRegData11);

  dsi_shortwrite (ShortRegData33);
  dsi_longwrite (LongRegData12);

  dsi_shortwrite (ShortRegData29);
  dsi_longwrite (LongRegData13);

  dsi_shortwrite (ShortRegData30);
  dsi_longwrite (LongRegData14);

  dsi_shortwrite (ShortRegData31);
  dsi_longwrite (LongRegData15);

  dsi_shortwrite (ShortRegData32);
  dsi_longwrite (LongRegData16);

  dsi_shortwrite (ShortRegData34);
  dsi_longwrite (LongRegData17);

  dsi_shortwrite (ShortRegData35);
  dsi_longwrite (LongRegData18);

  dsi_shortwrite (ShortRegData2);
  dsi_longwrite (LongRegData19);

  dsi_shortwrite (ShortRegData33);
  dsi_longwrite (LongRegData20);

  dsi_shortwrite (ShortRegData29);
  dsi_longwrite (LongRegData21);

  dsi_shortwrite (ShortRegData30);
  dsi_longwrite (LongRegData22);

  dsi_shortwrite (ShortRegData31);
  dsi_longwrite (LongRegData23);

  dsi_shortwrite (ShortRegData32);
  dsi_longwrite (LongRegData24);

  // PWR_CTRL1 - 0xc580h - 130th parameter - default 0x00 
  // pump 1 min and max DM                                
  dsi_shortwrite (ShortRegData13);
  dsi_shortwrite (ShortRegData47);
  dsi_shortwrite (ShortRegData48);
  dsi_shortwrite (ShortRegData49);

  // CABC LEDPWM frequency adjusted to 22,7kHz 
  dsi_shortwrite (ShortRegData50);
  dsi_shortwrite (ShortRegData51);

  // Exit CMD2 mode 
  dsi_shortwrite (ShortRegData1);
  dsi_shortwrite (ShortRegData25);

  // Standard DCS Initialization TO KEEP CAN BE DONE IN HSDT                   
  // NOP - goes back to DCS std command ? 
  dsi_shortwrite (ShortRegData1);

  // gamma correction 2.2+ table (HSDT possible) 
  dsi_shortwrite (ShortRegData1);
  dsi_longwrite (LongRegData3);

  // gamma correction 2.2- table (HSDT possible) 
  dsi_shortwrite (ShortRegData1);
  dsi_longwrite (LongRegData4);
          
  // send Sleep Out command to display : no parameter 
  dsi_shortwrite (ShortRegData36);
  
  // wait for sleep out exit 
  msleep (120);

  // set Pixel color format 
//dsi_shortwrite (ShortRegData37); // set Pixel color format to RGB565 - do not use
  dsi_shortwrite (ShortRegData38); // set Pixel color format to RGB888

  // send command to configure display in landscape orientation mode 
  // by default the orientation mode is portrait  
  // select orientation landscape 
  dsi_shortwrite (ShortRegData39);
  dsi_longwrite (LongRegData27);
  dsi_longwrite (LongRegData28);

  //* CABC : Content Adaptive Backlight Control section start >> 
  // note : defaut is 0 (lowest Brightness), 0xFF is highest Brightness, try 0x7F : intermediate value 
  dsi_shortwrite (ShortRegData40);

  // default is 0, try 0x2C - Brightness Control Block, Display Dimming & BackLight on 
  dsi_shortwrite (ShortRegData41);

  // default is 0, try 0x02 - image Content based Adaptive Brightness [Still Picture] 
  dsi_shortwrite (ShortRegData42);

  // default is 0 (lowest Brightness), 0xFF is highest Brightness 
  dsi_shortwrite (ShortRegData43);

  // enable tearing effect output signal - 1ms pulse, active high (scope)
  dsi_shortwrite (ShortRegData52);

  // CABC : Content Adaptive Backlight Control section end 
  // send Command Display On 
  dsi_shortwrite (ShortRegData44);

  // NOP command 
  dsi_shortwrite (ShortRegData1);

  // send Command GRAM memory write (no parameters) : this initiates frame write via other DSI commands sent by 
  // DSI host from LTDC incoming pixels in video mode 
  dsi_shortwrite (ShortRegData45);
}

