// file    : ft6336.h
// author  : rb
// purpose : header file for ft6336.c 
// date    : 251220
// last    : 251220

#ifndef __FT6336_H__
#define __FT6336_H__

// -- globals 
struct touch_state
{
  uint16_t x1_pos;
  uint16_t y1_pos;
  uint16_t x2_pos;
  uint16_t y2_pos;
  uint8_t num_points;
  uint8_t new_points;
} ts;

// -- defines
#define FT6336_I2C_ADR                     0x70  // 0b0111 0000 

#define FT6336_CHIP_ID                     0x64  // CIPHER
#define FT6336_VENDOR_ID                   0x11  // VENDORID - not working, always 0x00 - don' test code on this!
                                         
#define FT6336_REG_DEVICE_MODE             0x00  // device mode (0: working, 4: factory)
#define FT6336_REG_GESTURE_ID              0x01  // gesture ID               - always 0x00
#define FT6336_REG_TD_STATUS               0x02  // number of touch points (0..2)
                                           
#define FT6336_REG_TOUCH1_XH               0x03  // touch1 x hi-part (hi-nibble holds event flags) 
#define FT6336_REG_TOUCH1_XL               0x04  // touch1 x lo-part
#define FT6336_REG_TOUCH1_YH               0x05  // touch1 y hi-part (hi-nibble holds event flags) 
#define FT6336_REG_TOUCH1_YL               0x06  // touch1 y lo-part
                                           
#define FT6336_REG_TOUCH2_XH               0x09  // touch2 x hi-part (hi-nibble holds event flags)
#define FT6336_REG_TOUCH2_XL               0x0a  // touch2 x lo-part
#define FT6336_REG_TOUCH2_YH               0x0b  // touch2 y hi-part (hi-nibble holds event flags)
#define FT6336_REG_TOUCH2_YL               0x0c  // touch2 y lo-part

#define FT6336_REG_TOUCH1_WEIGHT           0x07  // touch1 weight            - always 0x00
#define FT6336_REG_TOUCH1_AREA             0x08  // touch1 misc/area         - always 0x00
#define FT6336_REG_TOUCH2_WEIGHT           0x0d  // touch2 weight            - always 0x00
#define FT6336_REG_TOUCH2_AREA             0x0e  // touch2 misc/area         - always 0xff

#define FT6336_REG_TH_GROUP                0x80  // touch threshold          - alwaus 0x00
#define FT6336_REG_PERIODACTIVE            0x88  // report rate active mode  - always 0x00
#define FT6336_REG_PERIODMONITOR           0x89  // report rate monitor mode - always 0x00
                                                                                     
#define FT6336_REG_LIB_VERSION             0xa1  // library version          - always 0x00
#define FT6336_REG_CIPHER                  0xa3  // chip ID / cipher         - always 0x00
#define FT6336_REG_GMODE                   0xa4  // interrupt mode           - always 0x00
#define FT6336_REG_FIRMID                  0xa6  // firmware ID              - always 0x00
#define FT6336_REG_VENDORID                0xa8  // vendor ID                - always 0x00
#define FT6336_REG_STATE                   0xbc  // current state            - always 0x00
                                                 

// -- prototypes
void init_ft6336 (void);

void ft6336_update (void);

uint8_t  ft6336_read_reg8  (uint8_t reg);
uint16_t ft6336_read_reg16 (uint8_t reg);

void ft6336_dump (void);

void ft6336_id (void);

#endif


