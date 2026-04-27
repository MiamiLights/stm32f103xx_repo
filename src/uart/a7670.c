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
    usart1_flush();
    // 1. Pulizia buffer RX prima di iniziare

    // 2. Impostazione modalità testo
    usart1_send_string("AT+CMGF=1\r");
    delay(500);
    usart1_flush(); // svuota risposta OK

    // 3. Comando invio SMS con destinatario (Usa solo \r)
    usart1_send_string("AT+CMGS=\"");
    usart1_send_string(number);
    usart1_send_string("\"\r");

    // 4. ATTESA REATTIVA DEL PROMPT '>' (MIGLIORATA)
    uint8_t c = 0;
        uint8_t prompt_trovato = 0;
        uint32_t timeout = 5000000; // Almeno mezzo secondo di timeout

    while(timeout > 0) {
        // Chiediamo se c'è un carattere. SE c'è, lo analizziamo.
        if (usart1_read_buffer(&c)) {
            if (c == '>') {
                prompt_trovato = 1;
                break;
            }
        }
        timeout--;
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
    uint8_t c = 0;
    uint32_t index = 0;

    memset(buffer, 0, length_max);

    usart1_flush();

    uint32_t timeout = 5000000;
    while(timeout > 0){
        if(usart1_read_buffer(&c)){
            if (c == '+') break;
        }
        timeout--;
    }

    if (timeout == 0) return; // Non è arrivato nulla

    buffer[0] = '+';
    index = 1;
    timeout = 5000000;

    while (index < length_max - 1 && timeout >0){
        if(usart1_read_buffer(&c)){
            if (c == '\n' || c == '\r') break;
            buffer[index] = c;
            index++;
            timeout = 5000000;
        } else{
            timeout--;
        }
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
