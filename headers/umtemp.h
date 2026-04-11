#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "../headers/main.h"

#define RCC_BASE 0x40021000UL
#define RCC_APB1RST (*(volatile uint32_t*)(RCC_BASE + 0x10UL)) // reset register (i2c)
#define RCC_APB1ENR (*(volatile uint32_t*)(RCC_BASE + 0x1CUL)) // clock enable register (i2c)
#define RCC_APB2RST (*(volatile uint32_t*)(RCC_BASE + 0x0CUL)) // reset register (PB)
//PB6 è I2C1_SCL
// Pb7 è I2C1_SDA
#define GPIOB_BASE 0x40010C00UL

#define I2C1_BASE 0x40005400UL
#define I2C1_CR1 (*(volatile uint32_t*)(I2C1_BASE + 0x00UL)) // enable peripheral
#define I2C1_CR2 (*(volatile uint32_t*)(I2C1_BASE +0x04UL)) // clock frequency register
#define I2C1_DR (*(volatile uint32_t*)(I2C1_BASE +0x10UL)) // data register
#define I2C1_TRISE (*(volatile uint32_t*)(I2C1_BASE +0x20UL)) // maximum rise time
#define I2C1_CCR (*(volatile uint32_t*)(I2C1_BASE +0x1CUL)) // clock speed
#define I2C1_SR1 (*(volatile uint32_t*)(I2C1_BASE +0x14UL)) // status register 1
#define I2C1_SR2 (*(volatile uint32_t*)(I2C1_BASE +0x18UL)) // status register 2

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

#endif
