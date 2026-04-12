#include "a7670.h"
#include "usart.h"
#include "common.h"
#include "timeout.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

void A7670E_poweron(void){
    // Configura PA8 come output push-pull 2MHz
    GPIOA_CRH &= ~(0b1111U << 0);
    GPIOA_CRH |= (0b0010U << 0);

    // PA8 a massa (PWRKEY LOW) per 1.5 secondi
    GPIOA_BSRR |= (1U<<24);
    delay(1500);

    // PA8 in alta impedenza (Floating Input) per rilasciare il tasto
    GPIOA_CRH &= ~(0b1111U << 0);
    GPIOA_CRH |= (0b0100U << 0);
}

void A7670E_send_sms(char* number, char* message){
    // 1. Pulizia buffer RX prima di iniziare
    while(USART1_SR & RXNE) (void)USART1_DR;

    // 2. Impostazione modalità testo
    usart1_send_string("AT+CMGF=1\r");
    delay(500);
    while(USART1_SR & RXNE) (void)USART1_DR; // Svuota l'OK di CMGF

    // 3. Comando invio SMS con destinatario (Usa solo \r)
    usart1_send_string("AT+CMGS=\"");
    usart1_send_string(number);
    usart1_send_string("\"\r");

    // 4. ATTESA REATTIVA DEL PROMPT '>' (MIGLIORATA)
    uint8_t c = 0;
    uint8_t prompt_trovato = 0;
    int timeout_count = 0;

    while(timeout_count < 5) {
        c = usart1_receive();

        if (c == '>') {
            prompt_trovato = 1;
            break; 
        }
        else if (c == 0xFF) {
            timeout_count++;
        }
        else {
            timeout_count = 0;
        }
    }

    if (prompt_trovato) {
        delay(100); 
        usart1_send_string(message);
        delay(100);
        usart1_send(0x1A); // Invio (Ctrl+Z)
    }

    delay(5000);
}

void A7670_read_gps_string(char* buffer, uint32_t length_max){
    char c = 0;
    uint32_t index = 0;

    for(uint32_t i = 0; i < length_max; i++) buffer[i] = '\0';

    for(int i = 0; i < 10; i++) {
        if (USART1_SR & RXNE) (void)USART1_DR;
        else break;
    }

    uint32_t search_limit = 2000;
    while(search_limit > 0){
        c = usart1_receive();
        if (c == '+') break;
        search_limit--;
    }

    if (c != '+') return;

    buffer[0] = '+';
    index = 1;

    while (index < length_max - 1){
        c = usart1_receive();
        if (c == '\n' || c == '\r') break;
        buffer[index] = c;
        index++;
    }
    buffer[index] = '\0';
}

void A7670_format_gps_data(char *buffer, GPS_Data *data) {
    A7670_read_gps_string(buffer, 100);
    if (buffer[0] != 0 && strncmp(buffer, "+CGPSINFO: ", 10)==0) {
        data->fixValid = 1;

        char *f;

        f = get_field(buffer, 0);
        if (f && *f != ',' && *f != '\0') data->latitude = atof(f);

        f = get_field(buffer, 2);
        if (f && *f != ',' && *f != '\0') data->longitude = atof(f);

        f = get_field(buffer, 5);
        if (f && *f != ',' && *f != '\0') {
            uint32_t time_int = atoi(f);
            data->second = time_int % 100;
            data->minute = (time_int / 100) % 100;
            data->hour = (time_int / 10000);
        }
    }
    else {
        data->fixValid = 4;
        return;
    }
}
