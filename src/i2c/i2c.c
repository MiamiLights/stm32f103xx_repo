#include "i2c.h"
#include "common.h"
#include "timeout.h"
#include <stdint.h>

void i2c1_init(void){
    // attivazione clock i2c1
    RCC_APB1ENR |= (1U << 21);

    // attivazione clock su PB
    RCC_APB2ENR |= (1U << 3);

    RCC_APB1RST |= (1U << 21); // reset i2c
    RCC_APB1RST &= ~(1U << 21); // reset spento
    RCC_APB2RST |= (1U << 3); // reset PB
    RCC_APB2RST &= ~(1U << 3); // reset spento

    // output alternate function open drain
    GPIOB_CRL &= ~(0xFUL << 24);
    GPIOB_CRL |= (0xFUL << 24);
    GPIOB_CRL &= ~(0xFUL << 28);
    GPIOB_CRL |= (0xFUL << 28);

    // disabilitiamo la periferica prima di scrivere
    I2C1_CR1 = 0;

    // setting clock frequency i2c
    I2C1_CR2 &= ~(0x3FUL);
    I2C1_CR2 |= (1U << 3); // 8MHz
    ticks_init(8);

    I2C1_CCR &= ~(0xFFFUL);
    I2C1_CCR |= (0x28);

    I2C1_TRISE &= ~(0x3FUL);
    I2C1_TRISE |= (0x9UL);

    I2C1_CR1 = I2C_CR1_PE;
}

void i2c1_start(uint8_t address, uint8_t direction){
    while (I2C1_SR2 & I2C_SR2_BUSY){}; // aspettiamo che la trasmissione sia finita

    // generiamo start condition
    I2C1_CR1 |= I2C_CR1_START;

    while (!(I2C1_SR1 & I2C_SR1_START_GENERATED)){};
    I2C1_DR = ((address << 1) | direction) ;

    while (!(I2C1_SR1 & I2C_SR1_ADDRESS_SENT)){
        if (I2C1_SR1 & I2C_SR1_AF) { // AF: Acknowledge failure
            I2C1_SR1 &= ~I2C_SR1_AF; // Clear AF
            I2C1_CR1 |= I2C_CR1_STOP; // Send STOP
            return;
        }
    };

    (void)I2C1_SR1;
    (void)I2C1_SR2;
}

void i2c1_write(uint8_t data){
    while(!(I2C1_SR1 & I2C_SR1_TXE_EMPTY)){};
    I2C1_DR = data;
}

void i2c1_stop(void){
    // Se siamo in trasmissione, aspettiamo che l'ultimo byte sia inviato (BTF)
    // Se siamo in ricezione, lo STOP è già stato richiesto in i2c1_read
    if (!(I2C1_SR1 & I2C_SR1_RXNE_NE)) {
        while(!(I2C1_SR1 & I2C_SR1_TXE_EMPTY)){};
        while(!(I2C1_SR1 & I2C_SR1_BTF_FINISHED)){};
    }
    I2C1_CR1 |= I2C_CR1_STOP; // stop condition
}

void i2c1_read(int n, uint8_t *data){
    I2C1_CR1 |= I2C_CR1_ACK; // attiviamo gli ack per i bit ricevuti
    for(int i = 0; i < n; i++){
        if (i == n-1){
            I2C1_CR1 &= ~I2C_CR1_ACK; // disattiviamo gli ack
            I2C1_CR1 |= I2C_CR1_STOP;
        }
        while(!(I2C1_SR1 & I2C_SR1_RXNE_NE)){};
        // Leggo il byte in DR
        data[i] = (uint8_t)I2C1_DR;
    }
}
