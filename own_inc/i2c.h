// file    : i2c.h
// author  : rb
// purpose : header file i2c.c 
// date    : 181010
// last    : 251223

#ifndef __I2C_H__
#define __I2C_H__

// -- defines

// -- prototypes
void init_i2c (void);

void init_i2c1 (void);
void init_i2c2 (void);
void init_i2c3 (void);
void init_i2c4 (void);

void i2c1_write_byte (uint8_t adr, uint8_t dat);
void i2c2_write_byte (uint8_t adr, uint8_t dat);
void i2c3_write_byte (uint8_t adr, uint8_t dat);
void i2c4_write_byte (uint8_t adr, uint8_t dat);

uint8_t i2c1_read_byte (uint8_t adr);
uint8_t i2c2_read_byte (uint8_t adr);
uint8_t i2c3_read_byte (uint8_t adr);
uint8_t i2c4_read_byte (uint8_t adr);

void i2c4_scan_bus (void);

void i2c1_dump_registers (void);
void i2c2_dump_registers (void);
void i2c3_dump_registers (void);
void i2c4_dump_registers (void);

#endif



