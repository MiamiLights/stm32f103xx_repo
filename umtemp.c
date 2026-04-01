#include <stdint.h>

#define RCC_BASE 0x40021000UL
#define RCC_APB1RST (*(volatile uint32_t*)(RCC_BASE + 0x10UL)) //109
#define RCC_APB1ENR (*(volatile uint32_t*)(RCC_BASE + 0x1CUL))
//PB6 è I2C1_SCL
// Pb7 è I2C1_SDA
#define GPIOB_BASE 0x40010C00UL
#define GPIOB_CRL (*(volatile uint32_t*)(GPIOB_BASE + 0x00UL))

#define I2C1_BASE 0x40005400UL
#define I2C1_CR1 (*(volatile uint32_t*)(I2C1_BASE + 0x00UL))
#define I2C1_CR2 (*(volatile uint32_t*)(I2C1_BASE +0x04UL))
#define I2C1_DR (*(volatile uint32_t*)(I2C1_BASE +0x10UL))
#define I2C1_TRISE (*(volatile uint32_t*)(I2C1_BASE +0x20UL))
#define I2C1_CCR (*(volatile uint32_t*)(I2C1_BASE +0x1CUL))


void resI2c1(void);
void enI2c1(void);

void resI2c1(void){
    RCC_APB1RST |= (1U<<21);
}

void enI2c1(void){
    RCC_APB1ENR |= (1U<<21);
}

void setPb6(void){
    //output open drain
    GPIOB_CRL &= ~(0xFUL << 24);
    GPIOB_CRL |= (0x1111UL << 24);
}

void setPb7(){
    GPIOB_CRL &= ~(0xFUL << 28);
    GPIOB_CRL |= (0x1111UL << 28);
}
