#ifndef GPS_TYPES_H
#define GPS_TYPES_H

#include <stdint.h>

typedef struct{
    double latitude;
    double longitude;
    int hour, minute, second;
    char fixValid;
} GPS_Data;

#endif
