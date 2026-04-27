#include "spi.h"
#include "common.h"
#include <stdint.h>

#define RX_BUFFER_SIZE 128
static volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
static volatile uint16_t buf_head = 0;
static volatile uint16_t buf_tauil = 0;

void SPI1_init(){
    /*
     * 1. configure BR
     * 2. select CPOL and CPHA bits
     * 3. select DFF bit per 8 bits data frame format
     * 4. LSBFIRST in cr1 (Nota: la SD usa MSB first, quindi va lasciato a 0)
     */


    RCC_APB2ENR |= (1U << 12); // SPI1 EN
    RCC_APB2ENR |= (1U << 2);  // GPIOA EN

    GPIOA_CRL &= ~(0xFFFFU << 16); // Pulizia registri per PA4, PA5, PA6 e PA7

    // PA4 (CS)   -> General Purpose Output Push-Pull 50MHz
    GPIOA_CRL |= (1U << 16);
    GPIOA_CRL |= (1U << 17);

    // PA5 (SCK)  -> Alternate Function Push-Pull 50MHz
    GPIOA_CRL |= (1U << 20);
    GPIOA_CRL |= (1U << 21);
    GPIOA_CRL |= (1U << 23);

    // PA6 (MISO) -> Input Floating
    GPIOA_CRL |= (1U << 26);

    // PA7 (MOSI) -> Alternate Function Push-Pull 50MHz
    GPIOA_CRL |= (1U << 28);
    GPIOA_CRL |= (1U << 29);
    GPIOA_CRL |= (1U << 31);

    // Mettiamo subito il CS (PA4) ad ALTO (SD a riposo)
    GPIOA_ODR |= (1U << 4);



    // 1. configure BR
    // fpclk/2 -> se il clk é 64MHZ allora 32 MHZ)
    // NOTA PER SD: Partiamo lenti a fpclk/256 altrimenti l'init fallisce!
    SPI1_CR1 |= (0b111 << 3);

    SPI1_CR1 |= (1U<<2); // master configuration

    // 2. select CPOL and CPHA bits
    SPI1_CR1 &= ~(1U << 1);
    // NOTA PER SD: Usa Modalità 0, quindi CPHA deve essere 0 (prima transizione)
    SPI1_CR1 &= ~(1U << 0);

    // 3. select DFF bit per 8 bits data frame format
    SPI1_CR1 &= ~(1U<<11);

    // GESTIONE MULTI-MASTER (Aggiunta per evitare l'errore hardware MODF)
    SPI1_CR1 |= (1U << 9); // SSM = 1 (Software Slave Management)
    SPI1_CR1 |= (1U << 8); // SSI = 1 (NSS interno forzato alto)

    SPI1_CR1 |= (1U<<6); // SPI enable
}


uint8_t SPI1_transmit(uint8_t data_out){
    /*
     * transmit parte quando si scrive dentro il butter Tx ossia dentro DR,
     * controllare chiaramente se il canale é libero e si può scrivere.
     * verificare eventuali flag per la corretta terminazione della comunicazione.
     */
    uint32_t timeout = 50000;

    while( (SPI1_SR & (1U<<7)) || !(SPI1_SR & (1U<<1)) ){ // canale occupato
        if (--timeout == 0) return 0xFF;
    }
    // canale non occupato
    SPI1_DR = data_out; // si pulisce la flag txne

    timeout = 50000;

    while( !(SPI1_SR & (1U<<0)) ){ // attendiamo il byte di risposta
        if (--timeout == 0) return 0xFF;
    }
    return SPI1_DR; // leggiamo il byte di risposta per resettare la flag RXNE
}
