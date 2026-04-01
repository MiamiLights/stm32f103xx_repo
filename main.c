#include "main.h"
#include "stdint.h"

int prova = 1;
int prova2;
const int var1 = 1;

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
        delay(500000);

        GPIOB_ODR &= ~(LED_PIN);
        delay(500000);
    }
}

int main(void) {
    LED_init();
}
