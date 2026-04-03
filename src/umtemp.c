#include <stdint.h>
#include "../headers/umtemp.h"

#define RCC_BASE 0x40021000UL
#define RCC_APB1RST (*(volatile uint32_t*)(RCC_BASE + 0x10UL)) // reset register (i2c)
#define RCC_APB1ENR (*(volatile uint32_t*)(RCC_BASE + 0x1CUL)) // clock enable register (i2c)
#define RCC_APB2ENR (*(volatile uint32_t*)(RCC_BASE + 0x18UL)) // clock enable register (PB)
#define RCC_APB2RST (*(volatile uint32_t*)(RCC_BASE + 0x0CUL)) // reset register (PB)
//PB6 è I2C1_SCL
// Pb7 è I2C1_SDA
#define GPIOB_BASE 0x40010C00UL
#define GPIOB_CRL (*(volatile uint32_t*)(GPIOB_BASE + 0x00UL))

#define I2C1_BASE 0x40005400UL
#define I2C1_CR1 (*(volatile uint32_t*)(I2C1_BASE + 0x00UL)) // enable peripheral
#define I2C1_CR2 (*(volatile uint32_t*)(I2C1_BASE +0x04UL)) // clock frequency register
#define I2C1_DR (*(volatile uint32_t*)(I2C1_BASE +0x10UL)) // data register
#define I2C1_TRISE (*(volatile uint32_t*)(I2C1_BASE +0x20UL)) // maximum rise time
#define I2C1_CCR (*(volatile uint32_t*)(I2C1_BASE +0x1CUL)) // clock speed
#define I2C1_SR1 (*(volatile uint32_t*)(I2C1_BASE +0x14UL)) // status register 1
#define I2C1_SR2 (*(volatile uint32_t*)(I2C1_BASE +0x18UL)) // status register 2

#define CR1_START (1U<<8)
#define SR2_BUSY (1U << 1)
#define SR1_START_GENERATED (1U << 0)
#define SR1_ADDRESS_SENT (1U<<1)
#define SR1_TXE_EMPTY (1U<<7)
#define SR1_BTF_FINISHED (1U<<2)
#define SR1_RXNE_NE (1U<<6)


void i2c1_init(void){

    // attivazione clock i2c1
    RCC_APB1ENR |= (1U << 21);

    //attivazione clock su PB
    RCC_APB2ENR |= (1U<<3);

    RCC_APB1RST |= (1U<<21); //reset i2c
    RCC_APB1RST &= ~(1U<<21); //reset spento
    RCC_APB2RST |= (1U<<3); //reset PB
    RCC_APB2RST &= ~(1U<<3); //reset spento

    //output alternate function open drain
    GPIOB_CRL &= ~(0xFUL << 24);
    GPIOB_CRL |= (0xFUL << 24);
    GPIOB_CRL &= ~(0xFUL << 28);
    GPIOB_CRL |= (0xFUL << 28);

    // disabilitiamo la periferica prima di scrivere
    I2C1_CR1 = 0;

    //setting clock frequency i2c
    I2C1_CR2 &= ~(0x3FUL);
    I2C1_CR2 |= (1U<<3); // 8MHz

    I2C1_CCR &= ~(0xFFFUL);
    I2C1_CCR |= (0x28);

    I2C1_TRISE &= ~(0x3FUL);
    I2C1_TRISE |= (0x9UL);

    I2C1_CR1 = 1;
}


void i2c1_start(uint8_t address, uint8_t direction){
    while (I2C1_SR2 & SR2_BUSY){}; // aspettiamo che la trasmissione sia finita

    //generiamo start condition
    I2C1_CR1 |= CR1_START;

    while (!(I2C1_SR1 & SR1_START_GENERATED)){};
    I2C1_DR = ((address << 1) | direction) ;

    while (!(I2C1_SR1 & SR1_ADDRESS_SENT)){
        if (I2C1_SR1 & (1U << 10)) { // AF: Acknowledge failure
            I2C1_SR1 &= ~(1U << 10); // Clear AF
            I2C1_CR1 |= (1U << 9);   // Send STOP
            return;
        }
    };

    (void)I2C1_SR1;
    (void)I2C1_SR2;
}


void i2c1_write(uint8_t data){
    while(!(I2C1_SR1& SR1_TXE_EMPTY)){};
    I2C1_DR = data;
}

void i2c1_stop(void){
    // Se siamo in trasmissione, aspettiamo che l'ultimo byte sia inviato (BTF)
    // Se siamo in ricezione, lo STOP è già stato richiesto in i2c1_read
    if (!(I2C1_SR1 & SR1_RXNE_NE)) {
        while(!(I2C1_SR1 & SR1_TXE_EMPTY)){};
        while(!(I2C1_SR1 & SR1_BTF_FINISHED)){};
    }
    I2C1_CR1 |= (1<<9); // stop condition
}

void i2c1_read(int n, uint8_t *data){
    I2C1_CR1 |= (1U<<10); //attiviamo gli ack per i bit ricevuti
    for(int i = 0; i < n; i++){
        if (i == n-1){
            I2C1_CR1 &= ~(1U<<10); //disattiviamo gli ack
            I2C1_CR1 |= (1U<<9);
        }
        while(!(I2C1_SR1 & SR1_RXNE_NE)){};
        //Leggo il byte in DR
        data[i] = (uint8_t)I2C1_DR;
    }
}

void delay_ms(uint32_t ms) {
    for(uint32_t j = 0; j < ms; j++) {
        for(uint32_t i = 0; i < 1000; i++) {
            __asm__("nop");
        }
    }
}

void calculate_data(uint8_t *buffer, AHT20_Data *data){
    uint32_t temp1 = buffer[3];
    uint32_t mask = 0xF;
    uint32_t raw_humidity = ((uint32_t) buffer[1] << 12) | ((uint32_t)buffer[2]<< 4 | (uint32_t)buffer[3] >> 4);
    uint32_t raw_temp = ((temp1 & mask)<< 16) | ((uint32_t)buffer[4]<< 8 | buffer[5] );

    data->humidity = ((float)raw_humidity/1048576.0f) * 100.0f;
    data->temperature = (((float)raw_temp/1048576.0f)*200.0f) - 50.0f;
}
