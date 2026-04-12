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
    uint32_t timeout = 4000000; // Timeout generoso

    while (timeout > 0){
        uint32_t sr = USART_SR;

        // Se c'è un errore (Overrun, Noise, Framing, Parity)
        if (sr & (1U << 3 | 1U << 2 | 1U << 1 | 1U << 0)) {
            (void)USART_SR; // Lettura SR
            (void)USART_DR; // Lettura DR per pulire i flag di errore
            // Non usciamo, continuiamo a cercare un byte valido
            continue;
        }

        // Se c'è un dato pronto
        if (sr & RXNE) {
            return (uint8_t)(USART_DR & 0xFF);
        }

        timeout--;
    };

    return 0xFF; // Timeout
}
/*
 * Carica nel buffer la stringa che viene trasmessa dal sensore in formato NMEA
 */
void A7670_read_gps_string(char* buffer, uint32_t length_max){
    char c = 0;
    uint32_t index = 0;

    for(uint32_t i = 0; i < length_max; i++) buffer[i] = '\0';

    // Svuotiamo preventivamente eventuali byte residui nella UART
    for(int i = 0; i < 10; i++) {
        if (USART_SR & RXNE) (void)USART_DR;
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
        //controllo in caso di più virgole o spazi vuoti conseguenti che altrimenti rovinerebbero la formattazione
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
    while(USART_SR & RXNE) (void)USART_DR;

    // 2. Impostazione modalità testo
    usart1_send_string("AT+CMGF=1\r");
    delay(500);
    while(USART_SR & RXNE) (void)USART_DR; // Svuota l'OK di CMGF

    // 3. Comando invio SMS con destinatario (Usa solo \r)
    usart1_send_string("AT+CMGS=\"");
    usart1_send_string(number);
    usart1_send_string("\"\r");

    // 4. ATTESA REATTIVA DEL PROMPT '>' (MIGLIORATA)
    uint8_t c = 0;
    uint8_t prompt_trovato = 0;
    int timeout_count = 0;

    // Leggiamo i caratteri finché non scattano 5 timeout consecutivi
    // (dando al modulo circa 2.5 secondi per elaborare la SIM)
    while(timeout_count < 5) {
        c = usart1_receive();

        if (c == '>') {
            prompt_trovato = 1;
            break; // Trovato il prompt, usciamo!
        }
        else if (c == 0xFF) {
            // La receive è andata a vuoto. Il modulo sta "pensando".
            timeout_count++;
        }
        else {
            // Abbiamo ricevuto un carattere (potrebbe essere l'eco o un \n).
            // Dato che il modulo sta comunicando, azzeriamo il contatore dei timeout.
            timeout_count = 0;
        }
    }

    // 5. Se abbiamo il prompt, inviamo il corpo del messaggio e Ctrl+Z
    if (prompt_trovato) {
        delay(100); // Pausa strategica dopo il prompt
        usart1_send_string(message);
        delay(100);
        usart1_send(0x1A); // Invio (Ctrl+Z)
    }

    // 6. Attesa finale per la trasmissione radio all'antenna
    delay(5000);
}

void NEO6M_format_gps_data(char *buffer, GPS_Data *data){
    NEO6M_read_gps_string(buffer, 100);
    if (buffer[0] != '\0' && strncmp(buffer, "$GNGGA", 6)==0){

        // $GNGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47

        // la stringa corrisponde
        char *f;

        // time
        f = get_field(buffer, 1);
        if(f && *f != ',' && *f != '\0'){
            uint32_t time = atof(f);
            data->second = time % 100;
            data->minute = (time / 100)%100;
            data->hour = (time/10000);
        }

        // latitudine
        f = get_field(buffer, 2);
        data->latitude = atoi(f);

        // longitudine
        f = get_field(buffer, 4);
        data->longitude = atoi(f);

        // fix quality
        f = get_field(buffer, 6);
        data->fixValid = atoi(f);
    }
}

void NEO6M_read_gps_string(char* buffer, uint32_t length_max){
    char c = 0;
    uint32_t index = 0;

    for(uint32_t i = 0; i < length_max; i++) buffer[i] = '\0';

    // Svuotiamo preventivamente eventuali byte residui nella UART
    for(int i = 0; i < 10; i++) {
        if (USART_SR & RXNE) (void)USART_DR;
        else break;
    }

    uint32_t search_limit = 2000;
    while(search_limit > 0){
        c = usart1_receive();
        if (c == '$') break;
        search_limit--;
    }

    if (c != '$') return;

    buffer[0] = '$';
    index = 1;

    while (index < length_max - 1){
        c = usart1_receive();
        if (c == '\n' || c == '\r') break;
        buffer[index] = c;
        index++;
    }
    buffer[index] = '\0';
}
