#ifndef NEO6M_H
#define NEO6M_H

#include <stdint.h>
#include "gps_types.h"

void NEO6M_read_gps_string(char* buffer, uint32_t length_max);
void NEO6M_format_gps_data(char *buffer, GPS_Data *data);

#endif
