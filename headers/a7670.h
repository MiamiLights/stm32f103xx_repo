#ifndef A7670_H
#define A7670_H

#include <stdint.h>
#include "gps_types.h"

void A7670E_poweron(void);
void A7670E_send_sms(char* number, char* message);
void A7670_read_gps_string(char* buffer, uint32_t length_max);
void A7670_format_gps_data(char *buffer, GPS_Data *data);

#endif
