#ifndef I2C_H
#define I2C_H

#include <stdint.h>

void i2c1_init(void);

void i2c1_start(uint8_t address, uint8_t direction);
void i2c1_write(uint8_t data);
void i2c1_read(int n, uint8_t *data);
void i2c1_stop(void);

void delay_ms(uint32_t ms);

#endif
