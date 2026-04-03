#include "../headers/main.h"
#include <stdint.h>
#include "../headers/umtemp.h"

void LED_init(void){

    RCC_APB2ENR |= GPIOBEN; //abilitiamo il clock per la porta B
    GPIOB_CRL &= ~(0xFUL << 8); // puliamo i bit di configurazione per PB2
    GPIOB_CRL |= (1U << 8); // modalità general purpose output push-pull

    GPIOB_ODR |= LED_PIN; // impostiamo il pin PB2 ad alto
}

AHT20_Data sensor_data;

int main(void) {
    // LED_init();

    uint8_t buffer[7];
    i2c1_init();
    AHT20_init_sequence();

    while (1) {
        AHT20_trigger_measurement();
        AHT20_read_results(buffer);
        calculate_data(buffer, &sensor_data);
        delay_ms(2000); // 2 secondi tra una lettura e l'altra
    }

}

void AHT20_init_sequence(void){
    uint8_t status;

    delay_ms(40);
    i2c1_start(0x38, 1);
    i2c1_read(1, &status);

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
    } while (status & 0x80); // Se status & 0x80 è vero, il sensore è ancora BUSY

    i2c1_start(0x38, 1);
    i2c1_read(7, buffer);
}
