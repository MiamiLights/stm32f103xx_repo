#ifndef CLOCK_SET
#define CLOCK_SET

#include <stdint.h>

#define RCC_BASE 0x40021000UL
#define FLASH_BASE 0x40022000UL
#define FLASH_ACR (*(volatile uint32_t*)(FLASH_BASE + 0x00UL))
#define RCC_CR (*(volatile uint32_t*)(RCC_BASE + 0x00UL))
#define RCC_CFGR (*(volatile uint32_t*)(RCC_BASE + 0x04UL))

uint8_t clock_setup();

#endif
