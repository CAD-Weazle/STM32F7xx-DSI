// file    : dsi.h
// author  : rb
// purpose : header file for dsi.c
// date    : 251205
// last    : 251225
//

#ifndef _DSI_H_
#define _DSI_H_

// -- defines
#define LCD_MEM_ADR      ((uint32_t)0xc0000000U) // start address SDRAM
//#define LCD_MEM_ADR    ((uint32_t)0x20007000U) // start address SRAM - small memory, test only

#define LCD_OTM8009A_ID                       0  // Virtual Channel ID 

// Serial Audio Interface (SAI) PLL is used also by LDTC
#define PLLSAIN                             417  // N = *417
#define PLLSAIQ                               4  // Q = /4         - not used
#define PLLSAIR                               5  // R = /5
#define PLLSAIP                               0  // P = 0b00 == /2 - not used

// display dimensions
#define HACT                                800  // horizontal number of pixels
#define VACT                                480  // vertical number of lines
#define XBORDER                             200  // for small SRAM test only
#define YBORDER                             150  // for small SRAM test only

// video sync signals are all 'fake' and only needed to interface LCTD with DSI Wrapper
#define HSYNC                                 1  // horizontal sync
#define HBP                                   1  // horizontal back porch
#define HFP                                   1  // horizontal front porch
#define VSYNC                                 1  // vertical sync 
#define VBP                                   1  // vertical back porch 
#define VFP                                   1  // vertical front porch

#define AHBP           (HSYNC + HBP        - 1)  // accumulated horizontal back porch
#define AAW            (HSYNC + HBP + HACT - 1)  // accumulated active width
#define AVBP           (VSYNC + VBP        - 1)  // accumulated vertical back porch 
#define AAH            (VSYNC + VBP + VACT - 1)  // accumulated active height

#define HSPOL                                 0  // horizontal sync polarity 
#define VSPOL                                 0  // vertical sync polarity
#define DEPOL                                 0  // data enable polarity
#define PCPOL                                 0  // pixel clock polarity

#define DSI_DCS_SHORT_PKT_WRITE_P0   0x00000005  // DCS short write, no parameters     
#define DSI_DCS_SHORT_PKT_READ       0x00000006  // DCS short read                     
#define DSI_DCS_SHORT_PKT_WRITE_P1   0x00000015  // DCS short write, one parameter     
#define DSI_DCS_LONG_PKT_WRITE       0x00000039  // DCS long write               
                                                                                       
#define DSI_GEN_SHORT_PKT_WRITE_P0   0x00000003  // Generic short write, no parameters 
#define DSI_GEN_SHORT_PKT_WRITE_P1   0x00000013  // Generic short write, one parameter 
#define DSI_GEN_SHORT_PKT_WRITE_P2   0x00000023  // Generic short write, two parameters
#define DSI_GEN_LONG_PKT_WRITE       0x00000029  // Generic long write 
#define DSI_GEN_SHORT_PKT_READ_P0    0x00000004  // Generic short read, no parameters  
#define DSI_GEN_SHORT_PKT_READ_P1    0x00000014  // Generic short read, one parameter  
#define DSI_GEN_SHORT_PKT_READ_P2    0x00000024  // Generic short read, two parameters 

// -- prototypes
void init_dsi (void);

void dsi_clocks_enable  (void);
void dsi_phy_enable     (void);
void dsi_adapted_config (void);
void ltdc_config        (void);
void dsi_host_config    (void);
void dsi_layer1_config  (void);

void dsi_refresh (void);

void dsi_shortwrite (const uint8_t dat[]);
void dsi_longwrite  (const uint8_t dat[]);

void dsi_clocks_dump  (void);
void dsi_phy_dump     (void);
void dsi_adapted_dump (void);
void ltdc_dump        (void);
void dsi_host_dump    (void);
void dsi_reg_dump     (void);

void dsi_fifo_test (void);
void fill_rect     (void);
void color_wheel   (void);

#endif
