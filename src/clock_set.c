#include "clock_set.h"
#include "stdint.h"
#include <stdint.h>

uint8_t clock_setup(){
    RCC_CR |= (1U<<0); // ci assicuriamo che hsi sia attivato
    while(!(RCC_CR & (1U<<1))){}; //attendiamo che l'oscilattore sia pronto

    FLASH_ACR &= ~(7U<<0);
    FLASH_ACR |= (1U<<1);

    // hclk not divided on apb2
    RCC_CFGR &= ~(7U<<13);

    // per questioni di sicurezza dividiamo il clock che arriva ad APB1 per 2
    RCC_CFGR &= ~(3U<<8);
    RCC_CFGR |= (1U<< 10);

    // setup PLL
    RCC_CFGR &= ~(1U<<16); // HSI/2
    RCC_CFGR &= ~(0xFUL<<18);
    RCC_CFGR |= (0b1110U<<18); // moltiplicatore x16

    RCC_CR |= (1U<<24); // PLL enable
    while(!(RCC_CR & 1U<<25)){}; // attendiamo che PLL sia pronto

    RCC_CFGR &= ~(3U<<0);
    RCC_CFGR |= (1U<<1); // selezioniamo pll come clock

    while((RCC_CFGR & (3U<<2)) != 2U<<2){}; // verifichiamo che lo switch sia avvenuto fisicamente
    return 64;
}
