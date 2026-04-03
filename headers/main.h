#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

// Registri GPIO e RCC (Spostati qui per chiarezza o riferiti se necessari)
#define GPIOB_BASE 0x40010C00UL
#define GPIOB_CRL (*(volatile uint32_t *)(GPIOB_BASE + 0x00UL))
#define GPIOB_ODR (*(volatile uint32_t *)(GPIOB_BASE + 0x0CUL))

#define RCC_BASE 0x40021000UL
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18UL))

#define GPIOBEN (1U << 3)
#define LED_PIN (1U << 2) // PB2

// Indirizzo I2C AHT20
#define AHT20_ADDR 0x38

// Prototipi
void delay(volatile uint32_t count);
void LED_init(void);
void LED_toggle(void);

void AHT20_init_sequence(void);
void AHT20_trigger_measurement(void);
void AHT20_read_results(uint8_t *buffer);
void AHT20_get_values(float *humidity, float *temperature);

#endif // MAIN_H
