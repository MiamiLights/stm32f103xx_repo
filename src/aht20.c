#include "aht20.h"
#include "i2c.h"
#include "timeout.h"
#include <stdint.h>

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

void AHT20_calculate_data(uint8_t *buffer, AHT20_Data *data){
    uint32_t temp1 = buffer[3];
    uint32_t mask = 0xF;
    uint32_t raw_humidity = ((uint32_t) buffer[1] << 12) | ((uint32_t)buffer[2]<< 4 | (uint32_t)buffer[3] >> 4);
    uint32_t raw_temp = ((temp1 & mask)<< 16) | ((uint32_t)buffer[4]<< 8 | buffer[5] );

    data->humidity = ((float)raw_humidity/1048576.0f) * 100.0f;
    data->temperature = (((float)raw_temp/1048576.0f)*200.0f) - 50.0f;
}
