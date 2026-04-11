#include "../headers/usart.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//pa8 ck
// pa9 tx
// pa10 rx
// pa11 cts
// pa12 rts

void usart1_init(uint8_t fclk, uint32_t baudrate){
    // diamo corrente a GPIOA e USART1
    RCC_APB2ENR |= (1U << 2) | (1U << 14);
    GPIOA_CRH &= ~((0b1111U << 4) | (0b1111U << 8));
    // PA9 output push pull open drain e PA10 input floating state
    GPIOA_CRH |= (0b1011 << 4) | (0b0100U << 8);

    //utilizziamo 8 bits
    USART_CR1 &= ~((1U<<12) | (1U<<10));
    USART_CR2 &= ~(3U<<12);// 1 stop bit

    uint32_t fclk_hz = (uint32_t)fclk * 1000000UL;
    uint32_t usartdiv = (fclk_hz + (baudrate / 2))/baudrate;

    USART_BRR = usartdiv;

    // USART enable, transmitter enable, receiver enable
    USART_CR1 |= (1U<<13) | (1U<<3) | (1U<<2);
}

void usart1_send(uint8_t data){
    while(!(USART_SR & TXE)){};
    USART_DR = data;
}

void usart1_send_string(char* data){
    while(*data != '\0'){
        usart1_send((uint8_t)(*data));
        data++;
    }
}

uint8_t usart1_receive(){
    uint32_t timeout = 500000;

    while (!(USART_SR & RXNE)){
        // Gestione errori (incluso Overrun) durante l'attesa
        if (USART_SR & (1U << 3 | 1U << 2 | 1U << 1 | 1U << 0)) {
            (void)USART_SR;
            (void)USART_DR;
        }
        if (--timeout == 0) return 0xFF; // Timeout
    };
    return (uint8_t)(USART_DR & 0xFF);
}

void read_gps_string(char* buffer, uint32_t length_max){
    char c = 0;
    uint32_t index = 0;

    for(uint32_t i = 0; i < length_max; i++) buffer[i] = '\0';

    // Svuotiamo preventivamente eventuali byte residui nella UART
    // per assicurarci di iniziare "freschi"
    for(int i = 0; i < 10; i++) {
        if (USART_SR & RXNE) (void)USART_DR;
        else break;
    }

    uint32_t search_limit = 1000;
    while(search_limit > 0){
        c = usart1_receive();
        if (c == '$') break;
        search_limit--;
    }

    if (search_limit == 0) return;

    buffer[0] = '$';
    index = 1;

    while (index < length_max - 1){
        c = usart1_receive();
        // Se timeout o terminatore, usciamo
        if (c == '\n' || c == '\r') break;
        buffer[index] = c;
        index++;
    }
    buffer[index] = '\0';
}

/**
 * restituisce un puntatore al primo carattere dopo la virgola in posizione 'index'
 **/
static char* get_field(char* str, int field_index) {
    int current_field = 0;
    char* p = str;
    if (field_index == 0) return str;
    while (*p) {
        if (*p == ',') {
            current_field++;
            if (current_field == field_index) return p + 1;
        }
        p++;
    }
    return NULL;
}

void format_gps_data(char *buffer, GPS_Data *data) {
    read_gps_string(buffer, 100);
    if (strncmp(buffer, "$GNGGA", 6)) {
        char *f;

        f = get_field(buffer, 1);
        //controllo in caso di più virgole o spazi vuoti conseguenti che altrimenti rovinerebbero la formattazione
        if (f && *f != ',' && *f != '\0') {
            uint32_t time_int = atoi(f);
            data->second = time_int % 100;
            data->minute = (time_int / 100) % 100;
            data->hour = (time_int / 10000);
        }

        f = get_field(buffer, 2);
        if (f && *f != ',' && *f != '\0') data->latitude = atof(f);

        f = get_field(buffer, 4);
        if (f && *f != ',' && *f != '\0') data->longitude = atof(f);

        f = get_field(buffer, 6);
        if (f && *f != ',' && *f != '\0') data->fixValid = atoi(f);
    }
}
