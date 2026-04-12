#include "../headers/main.h"
#include "../headers/clock_set.h"
#include "../headers/timeout.h"
#include "../headers/umtemp.h"
#include "../headers/usart.h"
#include <stdint.h>

void LED_init(void) {

  RCC_APB2ENR |= GPIOBEN;     // abilitiamo il clock per la porta B
  GPIOB_CRL &= ~(0xFUL << 8); // puliamo i bit di configurazione per PB2
  GPIOB_CRL |= (1U << 8);     // modalità general purpose output push-pull

  GPIOB_ODR |= LED_PIN; // impostiamo il pin PB2 ad alto
}

AHT20_Data sensor_data;
GPS_Data gpsData;
uint8_t receive;
uint8_t freq;
void A7670_test();
void A7670_init();

int main(void) {
  //LED_init();
  /*

  uint8_t buffer[7];
  i2c1_init();
  AHT20_init_sequence();

  while (1) {
      AHT20_trigger_measurement();
      AHT20_read_results(buffer);
      calculate_data(buffer, &sensor_data);
      delay(2000); // 2 secondi tra una lettura e l'altra
  }
  */
  freq = clock_setup();
  ticks_init(freq);
  usart1_init(freq, 9600);
  char buffer_gps[100];

  while (1) {
      // A7670_format_gps_data(buffer_gps, &gpsData);
      NEO6M_format_gps_data(buffer_gps, &gpsData);
  }
}

void AHT20_init_sequence(void) {
  uint8_t status;

  delay(40);
  i2c1_start(0x38, 1);
  i2c1_read(1, &status);

  if (!(status & (1U << 3))) {
    i2c1_start(0x38, 0);
    i2c1_write(0xBE);
    i2c1_write(0x08);
    i2c1_write(0x00);
    i2c1_stop();
    delay(10);
  }
}

void AHT20_trigger_measurement(void) {
  i2c1_start(0x38, 0);
  i2c1_write(0xAC);
  i2c1_write(0x33);
  i2c1_write(0x00);
  i2c1_stop();
}

void AHT20_read_results(uint8_t *buffer) {
  uint8_t status;

  delay(80);

  // Verifica se ha finito (Bit [7] deve essere 0)
  do {
    i2c1_start(0x38, 1);
    i2c1_read(1, &status);
  } while (status & 0x80); // Se status & 0x80 è vero, il sensore è ancora BUSY

  i2c1_start(0x38, 1);
  i2c1_read(7, buffer);
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
