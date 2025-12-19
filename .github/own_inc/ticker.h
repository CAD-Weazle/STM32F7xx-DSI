// file    : ticker.h
// author  : rb
// purpose : header file ticker.h
// date    : 170906
// last    : 251204

#ifndef __TICKER_H__
#define __TICKER_H__

#include "pll.h"

// -- globals
volatile extern int csec_elapsed;
volatile extern int dsec_elapsed;
volatile extern int sec_elapsed;

// -- defines 
// TCLK1 = 100Mc
#define TIMER_1HZ               (TCLK1/1 - 1) // Timer clock = 1Hz
#define TIMER_1KHZ           (TCLK1/1000 - 1) // Timer clock = 1kHz
#define TIMER_10KHZ         (TCLK1/10000 - 1) // Timer clock = 10kHz
#define TIMER_100KHZ       (TCLK1/100000 - 1) // Timer clock = 100kHz
#define TIMER_1MHZ        (TCLK1/1000000 - 1) // Timer clock = 1MHz
#define TIMER_TCLK                        (0) // Timer clock = TCLKx clock

// -- prototypes
void init_ticker (void);

void usleep (int16_t usecs);
void msleep (uint16_t msecs);

void timer_dump_registers (void);

#endif
