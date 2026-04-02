#include "main.h"
#include <stdint.h>
#include "umtemp.h"

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
    // LED_init();

    uint8_t buffer[7];
    i2c1_init();

}

void AHT20_init_sequence(void){
    uint8_t status;

    delay_ms(40);
    i2c1_start(0x38, 1);
    i2c1_read(1, &status);

    if(!(status & (1U << 3))){
        i2c1_write(0xBE);
        i2c1_write(0x08);
        i2c1_write(0x00);
        i2c1_stop();
    }
    delay_ms(10);
}

void AHT20_trigger_measurement(void){
    i2c1_start(0x38, 0);
    i2c1_write(0xAC);
    i2c1_write(0x33);
    i2c1_write(0x00);
    i2c1_stop();
}

void AHT20_read_results(uint8_t *buffer) {
    uint8_t status;

    delay_ms(80); // Punto 3: "Wait for 80ms"

    // Verifica se ha finito (Bit [7] deve essere 0)
    do {
        i2c1_start(0x38, 1);
        i2c1_read(1, &status);
        // Se status & 0x80 è vero, il sensore è ancora BUSY
    } while (status & 0x80);

    // Se arriviamo qui, Bit [7] è 0: "measurement is completed"
    // "and then six bytes can be read in a row"
    i2c1_start(0x38, 1);
    i2c1_read(6, buffer);
}
