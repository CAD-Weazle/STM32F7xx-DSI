// file    : ports.h
// author  : rb
// purpose : header file for ports.c
// date    : 170810
// last    : 251206
//

#ifndef _PORTS_H_
#define _PORTS_H_

// GPIO output set/clear macros
#define gpio_set(port, pin)       do {port->BSRR = (0x00000001 << pin);} while(0) // set outputs: lo-word BSRR
#define gpio_clr(port, pin)       do {port->BSRR = (0x00010000 << pin);} while(0) // clr outputs: hi-word BSRR

// GPIO port mode defines
#define GPIO_MODE_IN                (0x00000000u)  // GPIO set to input
#define GPIO_MODE_OUT               (0x00000001u)  // GPIO set to output
#define GPIO_MODE_AF                (0x00000002u)  // GPIO set to alternate function
#define GPIO_MODE_AN                (0x00000003u)  // GPIO set to analog mode

#define GPIO_OTYPE_PP               (0x00000000u)  // output push-pull
#define GPIO_OTYPE_OD               (0x00000001u)  // output open-drain

// port A
#define B_USER                                  1  // B_USER push button

// port B

// port C

// port H

// port J
#define LD_USER2                                5  // LD_USER2 - green LED
#define LD_USER1                               13  // LD_USER1 - red LED
#define DSI_RESET                              15  // DSI reset 

// port I
#define LCD_BL_CTRL                            14  // backlight TFT LCD

// -- prototypes
void init_ports (void);

void ports_stop (void);

void led_red_on     (void);
void led_red_off    (void);
void led_red_toggle (void);
void led_red_flash  (uint16_t delay);

void led_grn_on     (void);
void led_grn_off    (void);
void led_grn_toggle (void);
void led_grn_flash  (uint16_t delay);

void dsi_reset (void);
void tft_bl_on (void);

#endif



