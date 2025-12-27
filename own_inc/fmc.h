// file    : fmc.h
// author  : rb
// purpose : header file for fmc.c
// date    : 241224
// last    : 241224
//

#ifndef _FMC_H_
#define _FMC_H_

// -- defines
#define SDRAM_BASE_ADDR                  (0xc0000000L) // start address SDRAM
#define SDRAM_NUM_WORDS                     (0x400000) // SDRAM size in 32-bit words

#define SDRAM            ((uint32_t *)SDRAM_BASE_ADDR) // word pointer in SDRAM

 // SDRAM

// -- prototypes
void init_fmc (void);

void sdram_init (void);

void dump_fmc_regs (void);

int mats_plus (uint32_t base_addr, uint32_t words);

#endif
