#ifndef AHT20_H
#define AHT20_H

#include <stdint.h>

typedef struct{
    float temperature;
    float humidity;
} AHT20_Data;

void AHT20_init_sequence(void);
void AHT20_trigger_measurement(void);
void AHT20_read_results(uint8_t *buffer);
void AHT20_calculate_data(uint8_t *buffer, AHT20_Data *data);

#endif
