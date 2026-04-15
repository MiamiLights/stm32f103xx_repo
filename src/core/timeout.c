#include "timeout.h"
#include "common.h"

/* se vogliamo che il SysTick timer si resetti ogni ms dobbiamo impostare il numero
 * di cicli pari a freq * 1000000 * 0.001s.
 */
uint32_t ticks_init(uint32_t freq){
    uint32_t ticks = (freq * 1000U) -1U; // ticks da fare ogni ms

    // puliamo il valore passato in RVR
    SYSTICK_RVR = ticks;

    return ticks;
}

void delay(uint32_t delay_ms){

    SYSTICK_CVR = 0;

    // impostiamo come sorgente del clock il processore
    SYSTICK_CSR |= (1U<<2);

    // attiviamo systick
    SYSTICK_CSR |= 1;

    for(int i = 0; i < delay_ms; i++){
        while (!(SYSTICK_CSR & CSR_COUNTFLAG)){}
    }

    // disattiviamo systick
    SYSTICK_CSR &= ~1U;
}
