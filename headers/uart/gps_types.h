#ifndef GPS_TYPES_H
#define GPS_TYPES_H

typedef struct{
    double latitude;
    double longitude;
    int hour, minute, second;
    char fixValid;
} GPS_Data;

#endif
