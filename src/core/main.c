#include "main.h"
#include "clock_set.h"
#include "timeout.h"
#include "usart.h"
#include "a7670.h"
#include "neo6m.h"
#include "aht20.h"
#include <stdint.h>
#include "common.h"
#include "buzzer.h"

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
uint8_t freq;

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

int main(void) {
  freq = clock_setup();
  ticks_init(freq);

  usart1_init(freq, 9600);
  char buffer_gps[100];

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
  LED_init();
  TIM3_setup();

  while (1) {
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
