#include "usart.h"
#include "common.h"
#include <stdint.h>

void usart1_init(uint8_t fclk, uint32_t baudrate){
    // diamo corrente a GPIOA e USART1
    RCC_APB2ENR |= (1U << 2) | (1U << 14);
    GPIOA_CRH &= ~((0b1111U << 4) | (0b1111U << 8));
    // PA9 output push pull open drain e PA10 input floating state
    GPIOA_CRH |= (0b1011 << 4) | (0b0100U << 8);

    //utilizziamo 8 bits
    USART1_CR1 &= ~((1U<<12) | (1U<<10));
    USART1_CR2 &= ~(3U<<12);// 1 stop bit

    uint32_t fclk_hz = (uint32_t)fclk * 1000000UL;
    uint32_t usartdiv = (fclk_hz + (baudrate / 2))/baudrate;

    USART1_BRR = usartdiv;

    // USART enable, transmitter enable, receiver enable
    USART1_CR1 |= (1U<<13) | (1U<<3) | (1U<<2);
}

void usart1_send(uint8_t data){
    while(!(USART1_SR & TXE)){};
    USART1_DR = data;
}

void usart1_send_string(char* data){
    while(*data != '\0'){
        usart1_send((uint8_t)(*data));
        data++;
    }
}

uint8_t usart1_receive(){
    uint32_t timeout = 4000000; // Timeout generoso

    while (timeout > 0){
        uint32_t sr = USART1_SR;

        // Se c'è un errore (Overrun, Noise, Framing, Parity)
        if (sr & (1U << 3 | 1U << 2 | 1U << 1 | 1U << 0)) {
            (void)USART1_SR; // Lettura SR
            (void)USART1_DR; // Lettura DR per pulire i flag di errore
            // Non usciamo, continuiamo a cercare un byte valido
            continue;
        }

        // Se c'è un dato pronto
        if (sr & RXNE) {
            return (uint8_t)(USART1_DR & 0xFF);
        }

        timeout--;
    };

    return 0xFF; // Timeout
}
