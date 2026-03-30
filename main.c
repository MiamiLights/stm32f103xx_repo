#include "main.h"
#include <stdint.h>

void delay(volatile uint32_t count) {
    while(count--) {
        __asm("nop");
    }
}

void LED_init(void){

    RCC_APB2ENR |= GPIOBEN; //abilitiamo il clock per la porta B
    GPIOB_CRL &= ~(0xFUL << 8); // puliamo i bit di configurazione per PB2
    GPIOB_CRL |= (1U << 8); // modalità general purpose output push-pull

    while (1) {
        GPIOB_ODR |= LED_PIN; // impostiamo il pin PB2 ad alto
        delay(5000000);

        GPIOB_ODR &= ~(LED_PIN);
        delay(5000000);
    }
}
