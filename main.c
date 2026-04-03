#include "main.h"
#include <stdint.h>
#include <stdio.h>
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
    AHT20_Data sensor_data;

    AHT20_init_sequence();

    while (1) {
        AHT20_trigger_measurement();
        AHT20_read_results(buffer);
        calculate_data(buffer, &sensor_data);
        //printf("%f", sensor_data.humidity);
        //printf("%f", sensor_data.temperature);
        delay(5000000);
    }

}

void AHT20_init_sequence(void){
    uint8_t status;

    delay_ms(40);
    i2c1_start(0x38, 1);
    i2c1_read(1, &status);
    i2c1_stop();

    if(!(status & (1U << 3))){
        i2c1_start(0x38, 0);
        i2c1_write(0xBE);
        i2c1_write(0x08);
        i2c1_write(0x00);
        i2c1_stop();
        delay_ms(10);
    }
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

    delay_ms(80);

    // Verifica se ha finito (Bit [7] deve essere 0)
    do {
        i2c1_start(0x38, 1);
        i2c1_read(1, &status);
        i2c1_stop();



    } while (status & 0x80); // Se status & 0x80 è vero, il sensore è ancora BUSY

    i2c1_start(0x38, 1);
    i2c1_read(7, buffer);
    i2c1_stop();
}
