#include "usart.h"
#include "common.h"
#include <stdint.h>

#define RX_BUFFER_SIZE 128

static volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;

void usart1_init(uint8_t fclk, uint32_t baudrate){
    // diamo corrente a GPIOA e USART1
    RCC_APB2ENR |= (1U << 2) | (1U << 14);
    GPIOA_CRH &= ~((0b1111U << 4) | (0b1111U << 8));
    // PA9 output push pull open drain e PA10 input floating state
    GPIOA_CRH |= (0b1011 << 4) | (0b0100U << 8);

    //utilizziamo 8 bits
    USART1_CR1 &= ~((1U<<12) | (1U<<10));
    USART1_CR2 &= ~(3U<<12);// 1 stop bit
    USART1_CR1 |= (1U<<5); // abilitiamo interrupt RXNE
    NVIC_ISER1 |= (1U<<(37-32)); // abilitiamo interrupt nel controller NVIC (vettore 37 per usart1)

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

uint8_t usart1_read_buffer(uint8_t *data_out){
    if(rx_head == rx_tail){ // non ci sono dati nuovi da leggere
        return 0;
    }

    *data_out = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) & (RX_BUFFER_SIZE-1);
    return 1;
}

void USART1_IRQHandler(void){

    // utilizziamo una copia sicura che non farà resettare le flag;
    uint32_t sr = USART1_SR;

    // Controlliamo Overrun (bit 3), Noise (bit 2), Framing (bit 1), Parity (bit 0)
    if (sr & (1U << 3 | 1U << 2 | 1U << 1 | 1U << 0)) {
        (void)USART1_SR; // lettura SR
        (void)USART1_DR; // Lettura DR per pulire i flag di errore
        return;
    }
    // controlliamo se l'interrupt é stato causato dall'arrivo di dati
    if(sr & RXNE){
        // leggiamo il dato (questo pulisce il flag RXNE)
        uint8_t data = USART1_DR;

        rx_buffer[rx_head] = data;
        //Facciamo avanzare 'head' di un passo.
        // fa tornare l'indice a 0 se supera 127 (comportamento circolare)
        rx_head = (rx_head+1) & (RX_BUFFER_SIZE - 1);
    }
}

void usart1_flush(void) {
    // Disabilita solo l'interrupt di ricezione (RXNE) della USART1
    USART1_CR1 &= ~(1U << 5);

    rx_head = 0;
    rx_tail = 0;

    // Riabilita l'interrupt di ricezione
    USART1_CR1 |= (1U << 5);
}
