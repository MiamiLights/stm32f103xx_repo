#ifndef I2C_H
#define I2C_H

#include <stdint.h>

typedef struct{
    float temperature;
    float humidity;
}AHT20_Data ;

void i2c1_init(void);

void i2c1_start(uint8_t address, uint8_t direction);
void i2c1_write(uint8_t data);
void i2c1_read(int n, uint8_t *data);
void i2c1_stop(void);
void calculate_data(uint8_t *buffer, AHT20_Data *data);
void delay_ms(uint32_t ms);

#endif
