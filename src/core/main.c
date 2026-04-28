#include "main.h"
#include "clock_set.h"
#include "gps_types.h"
#include "timeout.h"
#include "usart.h"
#include "a7670.h"
#include "neo6m.h"
#include "SSD1306.h"
#include "aht20.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/_intsup.h>
#include "common.h"
#include "SD.h"
#include "spi.h"

void LED_init(void) {
  RCC_APB2ENR |= (1U << 3);   // abilitiamo il clock per la porta B (GPIOBEN)
  GPIOB_CRL &= ~(0xFUL << 8); // puliamo i bit di configurazione per PB2
  GPIOB_CRL |= (1U << 8);     // modalità general purpose output push-pull

  GPIOB_ODR &= ~LED_PIN;       // impostiamo il pin PB2 ad alto
}

void LED_toggle(void) {
    GPIOB_ODR ^= LED_PIN;
}

AHT20_Data sensor_data;
GPS_Data gpsData;
GPS_Data old_gpsData;
uint8_t freq;

char gpsBuffer[100] = {0};
char displayBuffer[1024] = {0};
char testo[30] = {0};
uint8_t sd_write_buffer[512];
uint8_t sd_read_buffer[512];

// Passiamo i puntatori (&old, &new) per risparmiare memoria
uint8_t is_display_update_needed(GPS_Data *old, GPS_Data *new){
    if (
        (old->hour != new->hour) ||
        (old->minute != new->minute) ||
        (old->second != new->second) ||
        (old->latitude != new->latitude) ||
        (old->longitude != new->longitude)
    ) {
        return 1;
    }
    return 0;
}

void format_gps_float(float value, char* buffer_out, const char* prefisso){
    int integer = (int)value;
    int decimal = (int) (((value - (float)integer)*100000.0f) + 0.5f);
    sprintf(buffer_out, "%s %d.%05d", prefisso, integer, decimal);
}

void update_display(){
    SSD1306_clear();
    memset(displayBuffer, 0, 1024);

    sprintf(testo, "Time: %02d:%02d:%02d", gpsData.hour, gpsData.minute, gpsData.second);
    SSD1306_print_string(testo, 0, 0, 1, displayBuffer);

    format_gps_float(gpsData.latitude, testo, "Lat: ");
    SSD1306_print_string(testo, 0, 10, 1, displayBuffer);


    format_gps_float(gpsData.longitude, testo, "Lon: ");
    SSD1306_print_string(testo, 0, 20, 1, displayBuffer);

    SSD1306_write_data(displayBuffer, 1024);
}


int main(void) {
    freq = clock_setup();
    ticks_init(freq);
    usart1_init(freq, 9600);
    SSD1306_power_on();
    old_gpsData = gpsData;
    SPI1_init();
    SSD1306_clear();
    memset(displayBuffer, 0, 1024);
    uint8_t stato_sd = sd_init();
    memset(sd_read_buffer, 0, 512);

    if (stato_sd != 0) {
        LED_init();
        memset(displayBuffer, 0, 1024);

        if (stato_sd == 1) {
            SSD1306_print_string("ERR: CMD0 MUTO", 0, 0, 1, displayBuffer);
        } else if (stato_sd == 2) {
            SSD1306_print_string("ERR: TIMEOUT 41", 0, 0, 1, displayBuffer);
        // dentro l'if (stato_sd != 0)
        } else if (stato_sd == 3) {
            SSD1306_print_string("ERR: CMD55 RIFIUTATO", 0, 0, 1, displayBuffer);
        }
        else {
            SSD1306_print_string("INIT FALLITO", 0, 0, 1, displayBuffer);
        }

        SSD1306_write_data(displayBuffer, 1024);
        while(1); // Blocca tutto qui per farti leggere l'errore
    }
    else {
        memset(displayBuffer, 0, 1024);
        SSD1306_print_string("INIT RIUSCITO", 0, 0, 1, displayBuffer);
        SSD1306_write_data(displayBuffer, 1024);
        delay(100);
    }
    delay(1000);
    memset(displayBuffer, 0, 1024);
    char* string = "PROVA SCHEDA SD";
    uint8_t i = 0;
    memset(sd_write_buffer, 0, 512);
    while(*string != '\0'){
        sd_write_buffer[i] = *string;
        string++;
        i++;
    }

    if (sd_write_sector(3, sd_write_buffer) != 0){
        SSD1306_print_string("SCRITTURA FALLITA", 0, 0, 1, displayBuffer);
        SSD1306_write_data(displayBuffer, 1024);
    }
    else{
        SSD1306_print_string("SCRITTURA RIUSCITA", 0, 0, 1, displayBuffer);
        SSD1306_write_data(displayBuffer, 1024);
        LED_toggle();
    }


    if(sd_read_sector(0, sd_read_buffer) == 0){
        char display_text[32];
        sniprintf(display_text, sizeof(display_text), "SD: %.16s", (char*)sd_read_buffer);
        SSD1306_print_string(display_text, 0, 20, 1, displayBuffer);
        SSD1306_write_data(displayBuffer, 1024);
    } else {
        SSD1306_print_string("ERRORE LETTURA", 0, 20, 1, displayBuffer);
        SSD1306_write_data(displayBuffer, 1024);
    }

    while (1) {
        // NEO6M_format_gps_data(gpsBuffer, &gpsData);
        //      if (is_display_update_needed(&old_gpsData, &gpsData)) {
        //          update_display();
        //          old_gpsData = gpsData;
        //      }
    }

}

void A7670_test(){
    A7670E_send_sms("+393314271084", "Ciao dal STM32!");

    delay(500);
    usart1_send_string("AT+CGNSSPWR=1\r");
    delay(500);
    usart1_send_string("AT+CGPSINFO=1\r");
    delay(500);
}

void A7670_init(){
    usart1_init(freq, 115200);
    A7670E_poweron();
    delay(15000);
}
