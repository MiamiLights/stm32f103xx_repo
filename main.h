#ifndef LED_H
#define LED_H

#define GPIOB_BASE 0x40010C00UL
#define GPIOB_CRL (*(volatile uint32_t *)(GPIOB_BASE + 0x00UL)) // regola la funzione della porta
#define GPIOB_ODR (*(volatile uint32_t*)(GPIOB_BASE + 0x0CUL)) // regola lo stato corrente del pin

#define RCC_BASE 0x40021000UL
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18UL))

#define GPIOBEN (1U<<3)
#define LED_PIN (1U<<2) // PB2

void LED_init(void);
void LED_Toggle(void);

#endif
