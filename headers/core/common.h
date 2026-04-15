#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

/* Base Addresses */
#define RCC_BASE      0x40021000UL
#define FLASH_BASE    0x40022000UL
#define GPIOA_BASE    0x40010800UL
#define GPIOB_BASE    0x40010C00UL
#define USART1_BASE   0x40013800UL
#define I2C1_BASE     0x40005400UL
#define SYSTICK_BASE  0xE000E010UL
#define DMA1_BASE     0x40020000UL
#define TIM3_BASE     0x40000400UL

/* RCC Registers */
#define RCC_CR        (*(volatile uint32_t*)(RCC_BASE + 0x00UL))
#define RCC_CFGR      (*(volatile uint32_t*)(RCC_BASE + 0x04UL))
#define RCC_APB2RST   (*(volatile uint32_t*)(RCC_BASE + 0x0CUL))
#define RCC_APB1RST   (*(volatile uint32_t*)(RCC_BASE + 0x10UL))
#define RCC_APB2ENR   (*(volatile uint32_t*)(RCC_BASE + 0x18UL))
#define RCC_APB1ENR   (*(volatile uint32_t*)(RCC_BASE + 0x1CUL))
#define RCC_AHBENR    (*(volatile uint32_t*)(RCC_BASE + 0x14UL))
#define RCC_AHBRSTR   (*(volatile uint32_t*)(RCC_BASE + 0x14UL))

/* FLASH Registers */
#define FLASH_ACR     (*(volatile uint32_t*)(FLASH_BASE + 0x00UL))

/* GPIOA Registers */
#define GPIOA_CRL     (*(volatile uint32_t*)(GPIOA_BASE + 0x00UL))
#define GPIOA_CRH     (*(volatile uint32_t*)(GPIOA_BASE + 0x04UL))
#define GPIOA_ODR     (*(volatile uint32_t*)(GPIOA_BASE + 0x0CUL))
#define GPIOA_BSRR    (*(volatile uint32_t*)(GPIOA_BASE + 0x10UL))

/* GPIOB Registers */
#define GPIOB_CRL     (*(volatile uint32_t*)(GPIOB_BASE + 0x00UL))
#define GPIOB_CRH     (*(volatile uint32_t*)(GPIOB_BASE + 0x04UL))
#define GPIOB_ODR     (*(volatile uint32_t*)(GPIOB_BASE + 0x0CUL))
#define GPIOB_BSRR    (*(volatile uint32_t*)(GPIOB_BASE + 0x10UL))

/* USART1 Registers */
#define USART1_SR     (*(volatile uint32_t*)(USART1_BASE + 0x00UL))
#define USART1_DR     (*(volatile uint32_t*)(USART1_BASE + 0x04UL))
#define USART1_BRR    (*(volatile uint32_t*)(USART1_BASE + 0x08UL))
#define USART1_CR1    (*(volatile uint32_t*)(USART1_BASE + 0x0CUL))
#define USART1_CR2    (*(volatile uint32_t*)(USART1_BASE + 0x10UL))

/* I2C1 Registers */
#define I2C1_CR1      (*(volatile uint32_t*)(I2C1_BASE + 0x00UL))
#define I2C1_CR2      (*(volatile uint32_t*)(I2C1_BASE + 0x04UL))
#define I2C1_OAR1     (*(volatile uint32_t*)(I2C1_BASE + 0x08UL))
#define I2C1_OAR2     (*(volatile uint32_t*)(I2C1_BASE + 0x0CUL))
#define I2C1_DR       (*(volatile uint32_t*)(I2C1_BASE + 0x10UL))
#define I2C1_SR1      (*(volatile uint32_t*)(I2C1_BASE + 0x14UL))
#define I2C1_SR2      (*(volatile uint32_t*)(I2C1_BASE + 0x18UL))
#define I2C1_CCR      (*(volatile uint32_t*)(I2C1_BASE + 0x1CUL))
#define I2C1_TRISE    (*(volatile uint32_t*)(I2C1_BASE + 0x20UL))

/* SysTick Registers */
#define SYSTICK_CSR   (*(volatile uint32_t *)(SYSTICK_BASE + 0x00UL))
#define SYSTICK_RVR   (*(volatile uint32_t *)(SYSTICK_BASE + 0x04UL))
#define SYSTICK_CVR   (*(volatile uint32_t *)(SYSTICK_BASE + 0x08UL))
#define SYSTICK_CALIB (*(volatile uint32_t *)(SYSTICK_BASE + 0x0CUL))

/* DMA Registers
 *
 * ISR -> Interrupt Status Register
 * IFCR -> Interrupt Flag Clear Register
 * CCR -> Channel x Configuration Register
 * CNDTR -> Channel x Number Of Data Register
 * CPAR -> Channel x Peripheral Address Register
 * CMAR -> Channel x Memory Address Register
 *
 */
#define DMA1_ISR            (*(volatile uint32_t *)(DMA1_BASE + 0x00UL))
#define DMA1_IFCR           (*(volatile uint32_t *)(DMA1_BASE + 0x04UL))
#define DMA1_CCR(channel)   (*(volatile uint32_t *)(DMA1_BASE + 0x08UL + (0x14UL * ((channel)-1))))
#define DMA1_CNDTR(channel) (*(volatile uint32_t *)(DMA1_BASE + 0x0CUL + (0x14UL * ((channel)-1))))
#define DMA1_CPAR(channel)  (*(volatile uint32_t *)(DMA1_BASE + 0x10UL + (0x14UL * ((channel)-1))))
#define DMA1_CMAR(channel)  (*(volatile uint32_t *)(DMA1_BASE + 0x14UL + (0x14UL * ((channel)-1))))

/* TIM Registers */
#define TIM3_CR1     (*(volatile uint32_t *)(TIM3_BASE + 0x00UL))
#define TIM3_CR2     (*(volatile uint32_t *)(TIM3_BASE + 0x04UL))
#define TIM3_DIER    (*(volatile uint32_t *)(TIM3_BASE + 0x0CUL))
#define TIM3_SR      (*(volatile uint32_t *)(TIM3_BASE + 0x10UL))
#define TIM3_CNT     (*(volatile uint32_t *)(TIM3_BASE + 0x24UL))
#define TIM3_PSC     (*(volatile uint32_t *)(TIM3_BASE + 0x28UL))
#define TIM3_ARR     (*(volatile uint32_t *)(TIM3_BASE + 0x2CUL))
#define TIM3_CCMR1   (*(volatile uint32_t *)(TIM3_BASE + 0x18UL))
#define TIM3_CCER    (*(volatile uint32_t *)(TIM3_BASE + 0x20UL))
#define TIM3_EGR     (*(volatile uint32_t *)(TIM3_BASE + 0x14UL))
#define TIM3_CCR2    (*(volatile uint32_t *)(TIM3_BASE + 0x38UL))


/* Common Bit Definitions */
#define TXE (1U<<7)
#define RXNE (1U<<5)

#define I2C_CR1_START (1U<<8)
#define I2C_CR1_STOP  (1U<<9)
#define I2C_CR1_ACK   (1U<<10)
#define I2C_CR1_PE    (1U<<0)

#define I2C_SR2_BUSY (1U << 1)
#define I2C_SR1_START_GENERATED (1U << 0)
#define I2C_SR1_ADDRESS_SENT (1U<<1)
#define I2C_SR1_TXE_EMPTY (1U<<7)
#define I2C_SR1_BTF_FINISHED (1U<<2)
#define I2C_SR1_RXNE_NE (1U<<6)
#define I2C_SR1_AF (1U << 10)

/* NVIC Registers */
#define NVIC_ISER0  (*(volatile uint32_t*)0xE000E100UL)
#define NVIC_ISER1  (*(volatile uint32_t*)0xE000E104UL)


#endif
