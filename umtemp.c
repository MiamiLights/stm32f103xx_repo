#include <stdint.h>

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


void i2c1_init(void){

    // attivazione clock i2c1
    RCC_APB1ENR |= (1U << 21);

    //attivazione clock su PB
    RCC_APB2ENR |= (1U<<3);

    RCC_APB1RST |= (1U<<21);
    RCC_APB2RST |= (1U<<3);

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


void i2c1_start(uint8_t address){
    while (!(I2C1_SR2 & SR2_BUSY)){}; // aspettiamo che la trasmissione sia finita

    //generiamo start condition
    I2C1_CR1 |= CR1_START;

    while (!(I2C1_SR1 & SR1_START_GENERATED)){};
    I2C1_DR = address;
}
