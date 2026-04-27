#ifndef SPI_H
#define SPI_H

#include <stdint.h>

#define SPI1_BASE 0x40013000UL
#define SPI1_CR1 (*(volatile uint32_t*)(SPI1_BASE + 0x00UL))
#define SPI1_CR2 (*(volatile uint32_t*)(SPI1_BASE + 0x04UL))
#define SPI1_SR  (*(volatile uint32_t*)(SPI1_BASE + 0x08UL))
#define SPI1_DR  (*(volatile uint32_t*)(SPI1_BASE + 0x0CUL))

void SPI1_init(void);
uint8_t SPI1_transmit(uint8_t data_out);

#endif
