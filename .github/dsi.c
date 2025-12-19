// file    : dsi.c
// author  : rb
// purpose : STM32F7xx LTDC/DSI routines 
// date    : 251205
// last    : 251219
//

#include "includes.h"

void init_dsi (void)
{
  // apply hw reset
  dsi_reset ();

  // config LTCD & DSI Host
  dsi_clocks_enable ();      
  dsi_phy_enable ();         
  dsi_adapted_config ();     
  ltdc_config ();            
  dsi_host_config ();        
  dsi_layer1_config ();      

  // enable interrupts 
//enable_interrupts ();      

  // dump some regs <> dev only
//dsi_dump_all ();
}

// enable LTDC & DSI Host
void dsi_clocks_enable (void)
{
  // enable clocks LTDC & DSI Host
  RCC->APB2ENR |= RCC_APB2ENR_LTDCEN    // LTDC clock enable
               |  RCC_APB2ENR_DSIEN;    // DSI Host clock enable

  // "The LTDC clock is generated from a specific PLL (PLLSAI). LTCD clock is not 
  // only used by LCD TFT controller but also by DSI Host." STM32F76x UM p.155
  // PLLSAI_VCO Input     = HSE_VALUE/PLL_M = 25Mc / 25 = 1Mc
  // PLLSAI_VCO Output    = PLLSAI_VCO Input * PLLSAIN = 1Mc * 417 = 417Mc
  // PLLLCDCLK            = PLLSAI_VCO Output/PLLSAIR = 417Mc / 5 = 83.4Mc
  // LTDC clock frequency = PLLLCDCLK / PLLSAIRDiv = 83.4 / 2 = 41.7Mc
  RCC->PLLSAICFGR = (PLLSAIN << RCC_PLLSAICFGR_PLLSAIN_Pos)   // N = *417
                  | (PLLSAIP << RCC_PLLSAICFGR_PLLSAIP_Pos)   // not used
                  | (PLLSAIQ << RCC_PLLSAICFGR_PLLSAIQ_Pos)   // not used
                  | (PLLSAIR << RCC_PLLSAICFGR_PLLSAIR_Pos);  // R = /5 

  // enable PLLSAI
  RCC->CR |= RCC_CR_PLLSAION;

  // wait PLL locked
  while (!(RCC->CR & RCC_CR_PLLSAIRDY))
    ;

  // turn on DSI voltage regulator
  DSI->WRPCR |= DSI_WRPCR_REGEN;

  // wait regulator stable
  while ((DSI->WISR & DSI_WISR_RRS) == 0)
    ;

  // setup DSI PLL
  // DSI_VCO = (HSE / IDF) * 2 * NDIV = 25Mc/5 * 2 * 100 = 1000Mc
  // DSI_PHY = DSI_VCO / (2 * ODF) = 1000/ (2 * 1) = 500Mc
  // DSI lane clock = DSI_PHY / 8 = 62.5Mc
  DSI->WRPCR &= ~(DSI_WRPCR_PLL_NDIV | DSI_WRPCR_PLL_IDF | DSI_WRPCR_PLL_ODF);  // clear bits
  DSI->WRPCR |= ( 0b00 << DSI_WRPCR_PLL_ODF_Pos)    // ODF : DSI PLL output division factor -  0b00 = /1
  		     |  (0b101 << DSI_WRPCR_PLL_IDF_Pos)    // IDF : DSI PLL input division factor  - 0b101 = /5
  			 |  (  100 << DSI_WRPCR_PLL_NDIV_Pos);  // NDIV: DSI PLL loop division factor

  // enable DSI PLL
  DSI->WRPCR |= DSI_WRPCR_PLLEN;

  // wait DSI PLL locked
  while ((DSI->WISR & DSI_WISR_PLLLS) == 0)
    ;
}

// setup DSI PHY
void dsi_phy_enable (void)
{
  // set PHY parameters
  DSI->PCTLR = DSI_PCTLR_CKE   // enable D-PHY clock lane module
             | DSI_PCTLR_DEN;  // enable D-PHY digital section

  // set clock lane to high-speed mode, no automatic clocklane control
  DSI->CLCR = DSI_CLCR_DPCC;

  // set number of active lanes & stop wait time
  DSI->PCONFR = (  10 << DSI_PCONFR_SW_TIME_Pos) // stop wait time
              | (0b01 << DSI_PCONFR_NL_Pos);     // two lanes

  // set DSI clock parameters
  // set TX Escape Clock divisor - max 20Mc 
  // DSI txclkesc = DSI lane clock / TXECKDIV = 62.5Mc / 4 = 15.625Mc
  DSI->CCR = (0 << DSI_CCR_TOCKDIV_Pos)    // HS/LP and LP/HS timeout clock not defined
           | (4 << DSI_CCR_TXECKDIV_Pos);

  // set UI (Unit Interval)
  DSI->WPCR[0] = 8; 

  // clear errors (if any)
  DSI->IER[0] = 0;
  DSI->IER[1] = 0;
}

// setup DSI Adapted Mode
void dsi_adapted_config (void)
{
  // set DSI to Command Mode
  DSI->MCR |= DSI_MCR_CMDM;

  // set color mode in DSI Wrapper (WFCGR) & DSI Host (LCOLCR)
  // DSI Wrapper color mode (this is LTDC -> DSI Host)
  DSI->WCFGR = (0b101 << DSI_WCFGR_COLMUX_Pos) // 24-bit color multiplexing
             | DSI_WCFGR_DSIM;                 // select adapted command mode

  // set virtual channel for the LTDC interface traffic
  DSI->LVCIDR = 0; // set virtual channel LTDC interface traffic
  DSI->GVCIDR = 0; // set virtual channel generic interface read-back

  // set polarity control signals
  DSI->LPCR = 0; // all active high

  // set DSI host color mode (this is DSI -> display)
  DSI->LCOLCR = 0b0101; // RGB888 mode

  // set maximum allowed size for write memory command
  DSI->LCCR = 800;

  // set all commands to High Speed
  // Acknowledge Request after each packet transmission disabled
  DSI->CMCR &= ~(DSI_CMCR_DLWTX  | DSI_CMCR_DSR0TX
            |    DSI_CMCR_DSW1TX | DSI_CMCR_DSW0TX
            |    DSI_CMCR_GLWTX  | DSI_CMCR_GSR2TX
            |    DSI_CMCR_GSR1TX | DSI_CMCR_GSR0TX
            |    DSI_CMCR_GSW2TX | DSI_CMCR_GSW1TX
            |    DSI_CMCR_GSW0TX);

  // enable Tearring Effect Request
//DSI->CMCR |= DSI_CMCR_TEARE;

  // enable interrupts
//DSI->WIER |= DSI_WIER_TEIE  // Tearing Effect interrupt
//          |  DSI_WIER_ERIE; // End of Refresh interrupt
}

// setup LTDC
void ltdc_config (void)
{
  // set polarity HS, VS, DE, PC 
  LTDC->GCR |= 0;  // all active low

  // set Synchronization size
  LTDC->SSCR = (HSYNC << 16) | VSYNC;

  // set Accumulated Back porch
  LTDC->BPCR = ((HSYNC+HBP) << 16) | (VSYNC+VBP); 

  // set Accumulated Active Width
  LTDC->AWCR = ((HSYNC+HBP+HACT) << 16) | (VSYNC+VBP+VACT);

  // set Total Width
  LTDC->TWCR = ((HSYNC+HBP+HACT+HFP) << 16) | (VSYNC+VBP+VACT+VFP);

  // set the background color value
  LTDC->BCCR = (0x00 << 16) | (0xff < 8) | 0x00; // some non-black color

  // enable Transfer Error & FIFO underrun interrupt
//LTDC->IER = (LTDC_IER_TERRIE | LTDC_IER_FUIE);

  // enable LTDC
  LTDC->GCR |= LTDC_GCR_LTDCEN;
}

// setup DSI Host
void dsi_host_config (void)
{
  // enable DSI Host
  DSI->CR |= DSI_CR_EN;
  usleep (100); // relax a bit (needed?)

  // enable DSI Wrapper
  DSI->WCR |= DSI_WCR_DSIEN;

  // set host clock lane timer configuration
  DSI->CLTCR = (35 << 16) | 35; 

  // data lane timer configuration 
  DSI->DLTCR = (35 << 24) | (35 << 16) | 0;

  // switch to HS transfer, enable Tearing Effect acknowledge request
  DSI->CMCR = DSI_CMCR_TEARE;

  // set DSI Host protocol configuration
  DSI->PCR |= DSI_PCR_BTAE;   // bus-turn-around enable
         //|  DSI_PCR_CRCRXE
         //|  DSI_PCR_ECCRXE
         //|  DSI_PCR_ETRXE
         //|  DSI_PCR_ETTXE;
}

// setup Layer 1, image data in SRAM (<> use SDRAM later)
void dsi_layer1_config (void)
{
  // set horizontal start and stop position - with border
//LTDC_Layer1->WHPCR = (HACT << 16) | 0;                 // full width when using SDRAM
  LTDC_Layer1->WHPCR = ((HACT-XBORDER) << 16) | XBORDER; // small area to fit SRAM

  // set vertical start and stop position
//LTDC_Layer1->WVPCR = (VACT << 16) |   0;               // full height when using SDRAM
  LTDC_Layer1->WVPCR = ((VACT-YBORDER) << 16) | YBORDER; // small area to fit SRAM

  // set pixel format
  LTDC_Layer1->PFCR = 0x000;  // ARGB8888
//LTDC_Layer1->PFCR = 0x001;  // RGB888

  // configure Layer1 default color
  LTDC_Layer1->DCCR = 0xffdaa520; // X11 Goldenrod

  // set constant alpha value
  LTDC_Layer1->CACR = 0xff;

  // set blending factors
  LTDC_Layer1->BFCR = 0x00000607; // no blending factor, keep at reset value

  // set frame buffer start address
  LTDC_Layer1->CFBAR = LCD_MEM_ADR;

  // set frame buffer pitch [byte]
  LTDC_Layer1->CFBLR  = (HACT << 16U) | (HACT*4  + 3);  // ARGB8888
//LTDC_Layer1->CFBLR  = (HACT << 16U) | (HACT*3  + 3);  // RGB888

  // set frame buffer line number
  LTDC_Layer1->CFBLNR  = VACT;

  // enable LTDC_Layer1
  LTDC_Layer1->CR |= LTDC_LxCR_LEN;

  // enable immediate reload
  LTDC->SRCR |= LTDC_SRCR_IMR;

  // enable DSI Wrapper
  DSI->WCR |= DSI_WCR_DSIEN;
  (void) DSI->WCR; 
}

// enable DSI & LTDC interrupts - not used now
void enable_interrupts (void)
{
  // LTDC interrupt
  NVIC_SetPriority (LTDC_IRQn, 3);
  NVIC_EnableIRQ (LTDC_IRQn);
 
  // DSI interrupt
  NVIC_SetPriority (DSI_IRQn, 3);
  NVIC_EnableIRQ (DSI_IRQn);
}

// enable LTDC frame transfer in Adapted Command Mode
void dsi_refresh (void)
{
  DSI->WCR |= DSI_WCR_LTDCEN;
}

// DSI DCS short write, one parameter
void dsi_shortwrite (const uint8_t dat[])
{
  // wait Command FIFO Empty
  while ((DSI->GPSR & DSI_GPSR_CMDFE) == 0)
    ;

  // write DSI packet header - channel ID & packet data type are constant
  DSI->GHCR = ((dat[1] << 16) | (dat[0] << 8) | (LCD_OTM8009A_ID << 6) | DSI_DCS_SHORT_PKT_WRITE_P1);
}

// DSI DCS long write, N parameters
void dsi_longwrite (const uint8_t dat[])
{
  uint32_t f_dat = 0;     // data FIFO accu
  uint8_t  packet_len;    // payload packet total length [bytes]
  uint32_t packet_num;    // chunk to be read next - 4 bytes or less
  uint32_t packet_ptr;    // current position in payload

  // setup reading packet
  packet_len = dat[0];    // packet length is stored in first positon (as per ST HAL example code)
  packet_ptr = 1;         // set first payload data location

  // wait Command FIFO Empty
  while ((DSI->GPSR & DSI_GPSR_CMDFE) == 0)
    ;

  // assemble 32-bit FIFO words - DCS command on LBS, rest is payload
  while (packet_len != 0)
  {
    // get next bytes from payload, max 4 bytes
    packet_num = (packet_len < 4) ? packet_len : 4;

    // clear accu
    f_dat = 0;

    // assemble 32-bit FIFO word
    for (int i = 0; i < packet_num; i++)
    {
      f_dat |= (uint32_t)dat[packet_ptr++] << (i*8);
    }

    // write accu to data FIFO
    DSI->GPDR = f_dat;

    // adjust packet lenght counter
    packet_len -= packet_num;
  }

  // get payload length, stored in first position
  uint32_t lsb =  (dat[0]) & 0x00ff;
  uint32_t msb = ((dat[0]) & 0xff00) >> 8; 

  // write DSI packet header  - channel ID & packet data type are constant
  DSI->GHCR = ((msb << 16) | (lsb << 8) | (LCD_OTM8009A_ID << 6) | DSI_DCS_LONG_PKT_WRITE);
}

// test data path - color a rectangle 
void fill_rect (void)
{
  static uint32_t color = 0;

//printf ("filling color: 0x%06lx\n", color);

  // fill memory
  for (int i = 0; i < ((HACT - 2*XBORDER) * (VACT - 2*YBORDER) + 4000)/4; i++) //??????
  {
    ((uint32_t *)LCD_MEM_ADR)[i] = 0xff000000 | color; 
  }

  // next color
  color += 16;
  color = (color % 0x00ffffff);

  // update image memory to display
  dsi_refresh ();
}



// dump LTDC & DSI Host clock/PLL registers
void dsi_clocks_dump (void)
{
  printf ("#dsi_clock_dump - Cube:\n");
  printf ("RCC->APB2ENR     : 0x%08lx\n", RCC->APB2ENR );
  printf ("RCC->APB2RSTR    : 0x%08lx\n", RCC->APB2RSTR);
  printf ("RCC->PLLSAICFGR  : 0x%08lx\n", RCC->PLLSAICFGR);
  printf ("RCC->CR          : 0x%08lx\n", RCC->CR);
  printf ("DSI->WRPCR       : 0x%08lx\n", DSI->WRPCR );
  printf ("DSI->WISR        : 0x%08lx\n", DSI->WISR);
}

// dump DSI PHY registers
void dsi_phy_dump (void)
{
  printf ("#dsi_phy_dump:\n");
  printf ("DSI->PCTLR       : 0x%08lx\n", DSI->PCTLR);
  printf ("DSI->CLCR        : 0x%08lx\n", DSI->CLCR);
  printf ("DSI->PCONFR      : 0x%08lx\n", DSI->PCONFR);
  printf ("DSI->CCR         : 0x%08lx\n", DSI->CCR);
  printf ("DSI->WPCR[0]     : 0x%08lx\n", DSI->WPCR[0]);
  printf ("DSI->IER[0]      : 0x%08lx\n", DSI->IER[0]);
  printf ("DSI->IER[1]      : 0x%08lx\n", DSI->IER[1]);
}

// dump DSI Adapted COmmand registers
void dsi_adapted_dump (void)
{
  printf ("#dsi_adapted_dump:\n");
  printf ("DSI->MCR         : 0x%08lx\n", DSI->MCR);
  printf ("DSI->WCFGR       : 0x%08lx\n", DSI->WCFGR);
  printf ("DSI->LVCIDR      : 0x%08lx\n", DSI->LVCIDR);
  printf ("DSI->GVCIDR      : 0x%08lx\n", DSI->GVCIDR);
  printf ("DSI->LPCR        : 0x%08lx\n", DSI->LPCR);
  printf ("DSI->LCOLCR      : 0x%08lx\n", DSI->LCOLCR);
  printf ("DSI->LCCR        : 0x%08lx\n", DSI->LCCR);
  printf ("DSI->CMCR        : 0x%08lx\n", DSI->CMCR);
}

// dump LTCD registers
void ltdc_dump (void)
{
  printf ("#ltcd_dump:\n");
  printf ("LTDC->SSCR       : 0x%08lx\n", LTDC->SSCR);   // LTDC synchronization size configuration register
  printf ("LTDC->BPCR       : 0x%08lx\n", LTDC->BPCR);   // LTDC back porch configuration register
  printf ("LTDC->AWCR       : 0x%08lx\n", LTDC->AWCR);   // LTDC active width configuration register
  printf ("LTDC->TWCR       : 0x%08lx\n", LTDC->TWCR);   // LTDC total width configuration register
  printf ("LTDC->GCR        : 0x%08lx\n", LTDC->GCR);    // LTDC global control register
  printf ("LTDC->SRCR       : 0x%08lx\n", LTDC->SRCR);   // LTDC shadow reload configuration register
  printf ("LTDC->BCCR       : 0x%08lx\n", LTDC->BCCR);   // LTDC background color configuration register
  printf ("LTDC->IER        : 0x%08lx\n", LTDC->IER);    // LTDC interrupt enable register
}

// dump DSI Host registers
void dsi_host_dump (void)
{
  printf ("#dsi_host_dump:\n");
  printf ("DSI->CR          : 0x%08lx\n", DSI->CR);
  printf ("DSI->WCR         : 0x%08lx\n", DSI->WCR);
  printf ("DSI->CLTCR       : 0x%08lx\n", DSI->CLTCR);
  printf ("DSI->DLTCR       : 0x%08lx\n", DSI->DLTCR);
  printf ("DSI->CMCR        : 0x%08lx\n", DSI->CMCR);
  printf ("DSI->PCR         : 0x%08lx\n", DSI->PCR);
}

// dump DSI registers
void dsi_reg_dump (void)
{
  printf ("DSI->VR     : 0x%08lx\n", DSI->VR);             // DSI Host Version Register,                                 Address offset: 0x00      
  printf ("DSI->CR     : 0x%08lx\n", DSI->CR);             // DSI Host Control Register,                                 Address offset: 0x04      
  printf ("DSI->CCR    : 0x%08lx\n", DSI->CCR);            // DSI HOST Clock Control Register,                           Address offset: 0x08      
  printf ("DSI->LVCIDR : 0x%08lx\n", DSI->LVCIDR);         // DSI Host LTDC VCID Register,                               Address offset: 0x0C      
  printf ("DSI->LCOLCR : 0x%08lx\n", DSI->LCOLCR);         // DSI Host LTDC Color Coding Register,                       Address offset: 0x10      
  printf ("DSI->LPCR   : 0x%08lx\n", DSI->LPCR);           // DSI Host LTDC Polarity Configuration Register,             Address offset: 0x14      
  printf ("DSI->LPMCR  : 0x%08lx\n", DSI->LPMCR);          // DSI Host Low-Power Mode Configuration Register,            Address offset: 0x18      
  printf ("DSI->PCR    : 0x%08lx\n", DSI->PCR);            // DSI Host Protocol Configuration Register,                  Address offset: 0x2C      
  printf ("DSI->GVCIDR : 0x%08lx\n", DSI->GVCIDR);         // DSI Host Generic VCID Register,                            Address offset: 0x30      
  printf ("DSI->MCR    : 0x%08lx\n", DSI->MCR);            // DSI Host Mode Configuration Register,                      Address offset: 0x34      
  printf ("DSI->VMCR   : 0x%08lx\n", DSI->VMCR);           // DSI Host Video Mode Configuration Register,                Address offset: 0x38      
  printf ("DSI->VPCR   : 0x%08lx\n", DSI->VPCR);           // DSI Host Video Packet Configuration Register,              Address offset: 0x3C      
  printf ("DSI->VCCR   : 0x%08lx\n", DSI->VCCR);           // DSI Host Video Chunks Configuration Register,              Address offset: 0x40      
  printf ("DSI->VNPCR  : 0x%08lx\n", DSI->VNPCR);          // DSI Host Video Null Packet Configuration Register,         Address offset: 0x44      
  printf ("DSI->VHSACR : 0x%08lx\n", DSI->VHSACR);         // DSI Host Video HSA Configuration Register,                 Address offset: 0x48      
  printf ("DSI->VHBPCR : 0x%08lx\n", DSI->VHBPCR);         // DSI Host Video HBP Configuration Register,                 Address offset: 0x4C      
  printf ("DSI->VLCR   : 0x%08lx\n", DSI->VLCR);           // DSI Host Video Line Configuration Register,                Address offset: 0x50      
  printf ("DSI->VVSACR : 0x%08lx\n", DSI->VVSACR);         // DSI Host Video VSA Configuration Register,                 Address offset: 0x54      
  printf ("DSI->VVBPCR : 0x%08lx\n", DSI->VVBPCR);         // DSI Host Video VBP Configuration Register,                 Address offset: 0x58      
  printf ("DSI->VVFPCR : 0x%08lx\n", DSI->VVFPCR);         // DSI Host Video VFP Configuration Register,                 Address offset: 0x5C      
  printf ("DSI->VVACR  : 0x%08lx\n", DSI->VVACR);          // DSI Host Video VA Configuration Register,                  Address offset: 0x60      
  printf ("DSI->LCCR   : 0x%08lx\n", DSI->LCCR);           // DSI Host LTDC Command Configuration Register,              Address offset: 0x64      
  printf ("DSI->CMCR   : 0x%08lx\n", DSI->CMCR);           // DSI Host Command Mode Configuration Register,              Address offset: 0x68      
  printf ("DSI->GHCR   : 0x%08lx\n", DSI->GHCR);           // DSI Host Generic Header Configuration Register,            Address offset: 0x6C      
  printf ("DSI->GPDR   : 0x%08lx\n", DSI->GPDR);           // DSI Host Generic Payload Data Register,                    Address offset: 0x70      
  printf ("DSI->GPSR   : 0x%08lx\n", DSI->GPSR);           // DSI Host Generic Packet Status Register,                   Address offset: 0x74      
  printf ("DSI->TCCR[0]: 0x%08lx\n", DSI->TCCR[0]);        // DSI Host Timeout Counter Configuration Register,           Address offset: 0x78-0x8F 
  printf ("DSI->TCCR[1]: 0x%08lx\n", DSI->TCCR[1]);        // DSI Host Timeout Counter Configuration Register,           Address offset: 0x78-0x8F 
  printf ("DSI->TCCR[2]: 0x%08lx\n", DSI->TCCR[2]);        // DSI Host Timeout Counter Configuration Register,           Address offset: 0x78-0x8F 
  printf ("DSI->TCCR[3]: 0x%08lx\n", DSI->TCCR[3]);        // DSI Host Timeout Counter Configuration Register,           Address offset: 0x78-0x8F 
  printf ("DSI->TCCR[4]: 0x%08lx\n", DSI->TCCR[4]);        // DSI Host Timeout Counter Configuration Register,           Address offset: 0x78-0x8F 
  printf ("DSI->TCCR[5]: 0x%08lx\n", DSI->TCCR[5]);        // DSI Host Timeout Counter Configuration Register,           Address offset: 0x78-0x8F 
  printf ("DSI->TDCR   : 0x%08lx\n", DSI->TDCR);           // DSI Host 3D Configuration Register,                        Address offset: 0x90      
  printf ("DSI->CLCR   : 0x%08lx\n", DSI->CLCR);           // DSI Host Clock Lane Configuration Register,                Address offset: 0x94      
  printf ("DSI->CLTCR  : 0x%08lx\n", DSI->CLTCR);          // DSI Host Clock Lane Timer Configuration Register,          Address offset: 0x98      
  printf ("DSI->DLTCR  : 0x%08lx\n", DSI->DLTCR);          // DSI Host Data Lane Timer Configuration Register,           Address offset: 0x9C      
  printf ("DSI->PCTLR  : 0x%08lx\n", DSI->PCTLR);          // DSI Host PHY Control Register,                             Address offset: 0xA0      
  printf ("DSI->PCONFR : 0x%08lx\n", DSI->PCONFR);         // DSI Host PHY Configuration Register,                       Address offset: 0xA4      
  printf ("DSI->PUCR   : 0x%08lx\n", DSI->PUCR);           // DSI Host PHY ULPS Control Register,                        Address offset: 0xA8      
  printf ("DSI->PTTCR  : 0x%08lx\n", DSI->PTTCR);          // DSI Host PHY TX Triggers Configuration Register,           Address offset: 0xAC      
  printf ("DSI->PSR    : 0x%08lx\n", DSI->PSR);            // DSI Host PHY Status Register,                              Address offset: 0xB0      
  printf ("DSI->ISR[0] : 0x%08lx\n", DSI->ISR[0]);         // DSI Host Interrupt & Status Register,                      Address offset: 0xBC-0xC3 
  printf ("DSI->ISR[1] : 0x%08lx\n", DSI->ISR[1]);         // DSI Host Interrupt & Status Register,                      Address offset: 0xBC-0xC3 
  printf ("DSI->IER[0] : 0x%08lx\n", DSI->IER[0]);         // DSI Host Interrupt Enable Register,                        Address offset: 0xC4-0xCB 
  printf ("DSI->IER[1] : 0x%08lx\n", DSI->IER[1]);         // DSI Host Interrupt Enable Register,                        Address offset: 0xC4-0xCB 
  printf ("DSI->FIR[0] : 0x%08lx\n", DSI->FIR[0]);         // DSI Host Force Interrupt Register,                         Address offset: 0xD8-0xDF 
  printf ("DSI->FIR[1] : 0x%08lx\n", DSI->FIR[1]);         // DSI Host Force Interrupt Register,                         Address offset: 0xD8-0xDF 
  printf ("DSI->VSCR   : 0x%08lx\n", DSI->VSCR);           // DSI Host Video Shadow Control Register,                    Address offset: 0x100     
  printf ("DSI->LCVCIDR: 0x%08lx\n", DSI->LCVCIDR);        // DSI Host LTDC Current VCID Register,                       Address offset: 0x10C     
  printf ("DSI->LCCCR  : 0x%08lx\n", DSI->LCCCR);          // DSI Host LTDC Current Color Coding Register,               Address offset: 0x110     
  printf ("DSI->LPMCCR : 0x%08lx\n", DSI->LPMCCR);         // DSI Host Low-power Mode Current Configuration Register,    Address offset: 0x118     
  printf ("DSI->VMCCR  : 0x%08lx\n", DSI->VMCCR);          // DSI Host Video Mode Current Configuration Register,        Address offset: 0x138     
  printf ("DSI->VPCCR  : 0x%08lx\n", DSI->VPCCR);          // DSI Host Video Packet Current Configuration Register,      Address offset: 0x13C     
  printf ("DSI->VCCCR  : 0x%08lx\n", DSI->VCCCR);          // DSI Host Video Chuncks Current Configuration Register,     Address offset: 0x140     
  printf ("DSI->VNPCCR : 0x%08lx\n", DSI->VNPCCR);         // DSI Host Video Null Packet Current Configuration Register, Address offset: 0x144     
  printf ("DSI->VHSACCR: 0x%08lx\n", DSI->VHSACCR);        // DSI Host Video HSA Current Configuration Register,         Address offset: 0x148     
  printf ("DSI->VHBPCCR: 0x%08lx\n", DSI->VHBPCCR);        // DSI Host Video HBP Current Configuration Register,         Address offset: 0x14C     
  printf ("DSI->VLCCR  : 0x%08lx\n", DSI->VLCCR);          // DSI Host Video Line Current Configuration Register,        Address offset: 0x150     
  printf ("DSI->VVSACCR: 0x%08lx\n", DSI->VVSACCR);        // DSI Host Video VSA Current Configuration Register,         Address offset: 0x154     
  printf ("DSI->VVBPCCR: 0x%08lx\n", DSI->VVBPCCR);        // DSI Host Video VBP Current Configuration Register,         Address offset: 0x158     
  printf ("DSI->VVFPCCR: 0x%08lx\n", DSI->VVFPCCR);        // DSI Host Video VFP Current Configuration Register,         Address offset: 0x15C     
  printf ("DSI->VVACCR : 0x%08lx\n", DSI->VVACCR);         // DSI Host Video VA Current Configuration Register,          Address offset: 0x160     
  printf ("DSI->TDCCR  : 0x%08lx\n", DSI->TDCCR);          // DSI Host 3D Current Configuration Register,                Address offset: 0x190     
  printf ("DSI->WCFGR  : 0x%08lx\n", DSI->WCFGR);          // DSI Wrapper Configuration Register,                       Address offset: 0x400      
  printf ("DSI->WCR    : 0x%08lx\n", DSI->WCR);            // DSI Wrapper Control Register,                             Address offset: 0x404      
  printf ("DSI->WIER   : 0x%08lx\n", DSI->WIER);           // DSI Wrapper Interrupt Enable Register,                    Address offset: 0x408      
  printf ("DSI->WISR   : 0x%08lx\n", DSI->WISR);           // DSI Wrapper Interrupt and Status Register,                Address offset: 0x40C      
  printf ("DSI->WIFCR  : 0x%08lx\n", DSI->WIFCR);          // DSI Wrapper Interrupt Flag Clear Register,                Address offset: 0x410      
  printf ("DSI->WPCR[0]: 0x%08lx\n", DSI->WPCR[0]);        // DSI Wrapper PHY Configuration Register,                   Address offset: 0x418-0x42B
  printf ("DSI->WPCR[1]: 0x%08lx\n", DSI->WPCR[1]);        // DSI Wrapper PHY Configuration Register,                   Address offset: 0x418-0x42B
  printf ("DSI->WPCR[2]: 0x%08lx\n", DSI->WPCR[2]);        // DSI Wrapper PHY Configuration Register,                   Address offset: 0x418-0x42B
  printf ("DSI->WPCR[3]: 0x%08lx\n", DSI->WPCR[3]);        // DSI Wrapper PHY Configuration Register,                   Address offset: 0x418-0x42B
  printf ("DSI->WPCR[4]: 0x%08lx\n", DSI->WPCR[4]);        // DSI Wrapper PHY Configuration Register,                   Address offset: 0x418-0x42B
  printf ("DSI->WRPCR  : 0x%08lx\n", DSI->WRPCR);          // DSI Wrapper Regulator and PLL Control Register, Address offset: 0x430                
  printf ("-------------------------------------------\n");
  printf ("RCC->PLLSAICFGR: 0x%08lx\n", RCC->PLLSAICFGR);  // SAI PLL settings
  printf ("-------------------------------------------\n");
//printf ("LTDC->TWCR          : 0x%08lx\n", LTDC->TWCR);
//printf ("LTDC->GCR           : 0x%08lx\n", LTDC->GCR);
//printf ("LTDC->SRCR          : 0x%08lx\n", LTDC->SRCR);
//printf ("LTDC->BCCR          : 0x%08lx\n", LTDC->BCCR);
//printf ("LTDC->IER           : 0x%08lx\n", LTDC->IER);
//printf ("LTDC->ISR           : 0x%08lx\n", LTDC->ISR);
//printf ("LTDC->ICR           : 0x%08lx\n", LTDC->ICR);
//printf ("LTDC->LIPCR         : 0x%08lx\n", LTDC->LIPCR);
//printf ("LTDC->CPSR          : 0x%08lx\n", LTDC->CPSR);
//printf ("LTDC->CDSR          : 0x%08lx\n", LTDC->CDSR);
//printf ("LTDC_Layer1->CR     : 0x%08lx\n", LTDC_Layer1->CR);
//printf ("LTDC_Layer1->WHPCR  : 0x%08lx\n", LTDC_Layer1->WHPCR);
//printf ("LTDC_Layer1->WVPCR  : 0x%08lx\n", LTDC_Layer1->WVPCR);
//printf ("LTDC_Layer1->CKCR   : 0x%08lx\n", LTDC_Layer1->CKCR);
//printf ("LTDC_Layer1->PFCR   : 0x%08lx\n", LTDC_Layer1->PFCR);
//printf ("LTDC_Layer1->DCCR   : 0x%08lx\n", LTDC_Layer1->DCCR);
//printf ("LTDC_Layer1->CFBAR  : 0x%08lx\n", LTDC_Layer1->CFBAR);
//printf ("LTDC_Layer1->CFBLR  : 0x%08lx\n", LTDC_Layer1->CFBLR);
//printf ("LTDC_Layer1->CFBLNR : 0x%08lx\n", LTDC_Layer1->CFBLNR);
//printf ("LTDC_Layer1->CLUTWR : 0x%08lx\n", LTDC_Layer1->CLUTWR);
//printf ("DONE\n");
}

// the GPDR data register connects to a FIFO apparently
// FIFO depth is not documented, let's find out
// results: Payload Write FIFO Full asserted after writting 65 32-bit words, DSI->GPSR: 0x00000019
// -> FIFO size: 64 32-bit words (256 bytes)
void dsi_fifo_test (void)
{
  int cnt = 0;

  // enable DSI Host clock
  RCC->APB2ENR |=  RCC_APB2ENR_DSIEN;

  // enable DSI Host
  DSI->CR |= DSI_CR_EN;
  usleep (100); // relax a bit (needed?)

  // check 'Payload Write FIFO Full' flag
  while ((DSI->GPSR & DSI_GPSR_PWRFF) == 0)
  {
    // current status
	printf ("write word %d, status: 0x%08lx\n", cnt+1, DSI->GPSR);

    // write dummy word
	DSI->GPDR = 0xdeadbeef;

    // update counter & relax a bit
	cnt++;
    msleep (10);
  }

  printf ("payload write FIFO full after %d 32-bit words, status: 0x%08lx\n", cnt+1, DSI->GPSR );
}

