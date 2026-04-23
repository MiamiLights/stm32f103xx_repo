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

void LED_init(void) {
  RCC_APB2ENR |= (1U << 3);   // abilitiamo il clock per la porta B (GPIOBEN)
  GPIOB_CRL &= ~(0xFUL << 8); // puliamo i bit di configurazione per PB2
  GPIOB_CRL |= (1U << 8);     // modalità general purpose output push-pull

  GPIOB_ODR |= LED_PIN;       // impostiamo il pin PB2 ad alto
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

void update_display(){
    SSD1306_clear();
    memset(displayBuffer, 0, 1024);
    sprintf(testo, "Time: %02d:%02d:%02d", gpsData.hour, gpsData.minute, gpsData.second);
    SSD1306_print_string(testo, 0, 0, 1, displayBuffer);
    sprintf(testo, "Lat: %f", gpsData.latitude);
    SSD1306_print_string(testo, 0, 10, 1, displayBuffer);
    SSD1306_write_data(displayBuffer, 1024);
}


int main(void) {
  freq = clock_setup();
  ticks_init(freq);
  usart1_init(freq, 9600);
  SSD1306_power_on();
  old_gpsData = gpsData;

  NEO6M_format_gps_data(gpsBuffer, &gpsData);
  SSD1306_clear();
  memset(displayBuffer, 0, 1024);
  SSD1306_print_string("Time: 20:20:12", 0, 0, 1, displayBuffer);
  SSD1306_write_data(displayBuffer, 1024);

  //update_display();

  /*
  // Esempio I2C per AHT20
  i2c1_init();
  AHT20_init_sequence();
  uint8_t buffer_i2c[7];
  */

  /*
  RCC_APB2ENR |= (1U << 2);
  GPIOA_CRL &= ~(0xFU << 28);
  GPIOA_CRL |= (0b0010U << 28);
  */

  while (1) {
      NEO6M_read_gps_string(gpsBuffer, 100);
      NEO6M_format_gps_data(gpsBuffer, &gpsData);
      if (is_display_update_needed(&old_gpsData, &gpsData)) {
          update_display();
          old_gpsData = gpsData;
      }


      /*
      GPIOA_ODR |= (1U<<7);
      delay(500);
      GPIOA_ODR &= ~(1U<<7);
      delay(500);
      NEO6M_format_gps_data(buffer_gps, &gpsData);
      A7670_format_gps_data(buffer_gps, &gpsData);

      AHT20_trigger_measurement();
      AHT20_read_results(buffer_i2c);
      AHT20_calculate_data(buffer_i2c, &sensor_data);
      */

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
