// file    : fmc.c
// author  : rb
// purpose : STM32F7xx FMC routines 
// date    : 251224
// last    : 251225
//
// note    : SDRAM on STM32F769I-DISCO board: IS42S32400F - 128Mb/1M x 32 bit x 4 banks (0x00400000 32-bit words

#include "includes.h"

// setup FMC
void init_fmc (void) 
{
  // setup SDRAM
  sdram_init ();

  // run MATS+ on entire SDRAM
//printf ("MATS+: %d\n", mats_plus (SDRAM_BASE_ADDR, SDRAM_NUM_WORDS));
}

// setup IS42S32400F 128Mb/4Mx32 SDRAM in Bank 1
void sdram_init (void) 
{
  volatile uint32_t tmp = 0;

  // enable FMC clock
  RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;
  
  // setup SDRAM Control Register
  FMC_Bank5_6->SDCR[0] = (0b0  << FMC_SDCR1_RPIPE_Pos)     // no read pipe delay
                       | (0b1  << FMC_SDCR1_RBURST_Pos)    // single reads always managed as bursts
                       | (0b10 << FMC_SDCR1_SDCLK_Pos)     // SDCLK = HCLK/2 
                       | (0b0  << FMC_SDCR1_WP_Pos)        // write access allowed
                       | (0b11 << FMC_SDCR1_CAS_Pos)       // CAS latency = 2 clock cycles
                       | (0b1  << FMC_SDCR1_NB_Pos)        // 4 internal banks
                       | (0b10 << FMC_SDCR1_MWID_Pos)      // 32-bit memory data bus
                       | (0b01 << FMC_SDCR1_NR_Pos)        // 12 row address bits
                       | (0b00 << FMC_SDCR1_NC_Pos);       // 8 column address bits

  // setup SDRAM Timing Register
  FMC_Bank5_6->SDTR[0] = (0b0001 << FMC_SDTR1_TRCD_Pos)    // row to column delay         : 2 clock cycles 
                       | (0b0001 << FMC_SDTR1_TRP_Pos)     // row precharge delay         : 2 clock cycles 
                       | (0b0001 << FMC_SDTR1_TWR_Pos)     // write recovery time         : 2 clock cycles 
                       | (0b0110 << FMC_SDTR1_TRC_Pos)     // row cycle delay             : 7 clock cycles 
                       | (0b0011 << FMC_SDTR1_TRAS_Pos)    // self refresh time           : 4 clock cycles 
                       | (0b0110 << FMC_SDTR1_TXSR_Pos)    // exit self-refresh delay     : 7 clock cycles 
                       | (0b0001 << FMC_SDTR1_TMRD_Pos);   // load mode register to active: 2 clock cycles 

  // -- SDRAM initialization sequence

  // send Clock Enable command 
  FMC_Bank5_6->SDCMR = (0b0000 << FMC_SDCMR_MRD_Pos)       // not used
                     | (0b0000 << FMC_SDCMR_NRFS_Pos)      // not used
                     | (0b1    << FMC_SDCMR_CTB1_Pos)      // Bank 1 target              - register bits write-only
                     | (0b0    << FMC_SDCMR_CTB2_Pos)      // Bank 2 not used            - register bits write-only
                     | (0b001  << FMC_SDCMR_MODE_Pos);     // Clock Configuration Enable - register bits write-only 

  // wait while SDRAM busy
  while ((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) != 0)
    ;

  // relax a bit - as per ST example code
  usleep (200);

  // send PALL command
  FMC_Bank5_6->SDCMR = (0      << FMC_SDCMR_MRD_Pos)       // not used
                     | (0b0000 << FMC_SDCMR_NRFS_Pos)      // not used
                     | (0b1    << FMC_SDCMR_CTB1_Pos)      // Bank 1 target              - register bits write-only
                     | (0b0    << FMC_SDCMR_CTB2_Pos)      // Bank 2 not used            - register bits write-only
                     | (0b010  << FMC_SDCMR_MODE_Pos);     // 'All Banks Precharge'      - register bits write-only 

  // wait while SDRAM busy
  while ((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) != 0)
    ;
  
  // send Auto-refresh command 
  FMC_Bank5_6->SDCMR = (0      << FMC_SDCMR_MRD_Pos)       // not used
                     | (0b0111 << FMC_SDCMR_NRFS_Pos)      // 8 Auto-refresh
                     | (0b1    << FMC_SDCMR_CTB1_Pos)      // Bank 1 target              - register bits write-only
                     | (0b0    << FMC_SDCMR_CTB2_Pos)      // Bank 2 not used            - register bits write-only
                     | (0b011  << FMC_SDCMR_MODE_Pos);     // Auto-refresh command       - register bits write-only 

  // wait while SDRAM busy
  while ((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) != 0)
    ;

  // setup Mode Register definition
  FMC_Bank5_6->SDCMR = (0x230  << FMC_SDCMR_MRD_Pos)       // Mode Register definition
                     | (0b0000 << FMC_SDCMR_NRFS_Pos)      // not used
                     | (0b1    << FMC_SDCMR_CTB1_Pos)      // Bank 1 target              - register bits write-only
                     | (0b0    << FMC_SDCMR_CTB2_Pos)      // Bank 2 not used            - register bits write-only
                     | (0b100  << FMC_SDCMR_MODE_Pos);     // load Mode Register         - register bits write-only 

  // wait while SDRAM busy
  while ((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) != 0)
    ;

  // set SDRAM refresh count
  tmp = FMC_Bank5_6->SDRTR;
  FMC_Bank5_6->SDRTR = (tmp | (0x00000603<<1));            // Refresh Timer Count = 1538 
  
  // disable write protection - as per ST example, not really needed, WP is bit cleared already
  tmp = FMC_Bank5_6->SDCR[0]; 
  FMC_Bank5_6->SDCR[0] = (tmp & ~FMC_SDCR1_WP);
 
  // ST: disable FMC Bank 1 (enabled after reset)
  // - this prevents CPU speculative reads on this bank which blocks the use of FMC during
  // - 24us, during this time the others using FMC master (such as LTDC) cannot use it
  FMC_Bank1->BTCR[0] = 0x000030d2;
}

// dump SDRAM registers
void dump_fmc_regs (void)
{
  printf ("FMC regs:\n");
  // SDRAM registers (FMC_Bank5_6)
  printf ("FMC_SDCR1  0x%08lx\n", FMC_Bank5_6->SDCR[0]);  // SDRAM Control Register Bank 1
  printf ("FMC_SDTR1  0x%08lx\n", FMC_Bank5_6->SDTR[0]);  // SDRAM Timing Register Bank 1
  printf ("FMC_SDCMR  0x%08lx\n", FMC_Bank5_6->SDCMR);    // SDRAM Command Mode Register
  printf ("FMC_SDRTR  0x%08lx\n", FMC_Bank5_6->SDRTR);    // SDRAM Refresh Timer Register
//printf ("FMC_SDCR2  0x%08lx\n", FMC_Bank5_6->SDCR[1]);  // SDRAM Control Register Bank 2
//printf ("FMC_SDTR2  0x%08lx\n", FMC_Bank5_6->SDTR[1]);  // SDRAM Timing Register Bank 2
  printf ("FMC_SDSR   0x%08lx\n", FMC_Bank5_6->SDSR);     // SDRAM Status Register (read-only)
}


// ---------- dev only ----------

#define PAT0 0x00000000U
#define PAT1 0xffffffffU

static inline void mem_barrier(void)
{
  __asm__ volatile ("" ::: "memory");
}

// MATS+ memory test - AI generated
int mats_plus (uint32_t base_addr, uint32_t words)
{
  volatile uint32_t *mem = (volatile uint32_t *)base_addr;

  // lo to hi: write 0 
  for (int i = 0; i < words; i++) 
    mem[i] = PAT0;
  
  mem_barrier();
  
  // lo to hi: read 0, write 1 
  for (int i = 0; i < words; i++) 
  {
    if (mem[i] != PAT0) 
      return 1;
  
    mem[i] = PAT1;
  }
  
  mem_barrier();
  
  // hi to lo: read 1, write 0 
  for (int i = words; i-- > 0; ) 
  {
    if (mem[i] != PAT1) 
      return 2;
  
    mem[i] = PAT0;
  }
  
  mem_barrier();
  
  // lo to hi: read 0 
  for (int i = 0; i < words; i++) 
  {
    if (mem[i] != PAT0) 
      return 3;
  }
  
  printf ("PASS\n");
  
  return 0; // PASS
}




