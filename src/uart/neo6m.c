#include "neo6m.h"
#include "usart.h"
#include "common.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * restituisce un puntatore al primo carattere dopo la virgola in posizione 'index'
 **/
static char* get_field(char* str, int field_index) {
    int current_field = 0;
    char* p = str;
    if (field_index == 0) return str;
    while (*p) {
        if (*p == ',') {
            current_field++;
            if (current_field == field_index) return p + 1;
        }
        p++;
    }
    return NULL;
}

void NEO6M_read_gps_string(char* buffer, uint32_t length_max){
    uint8_t c = 0;
    uint32_t index = 0;
    memset(buffer, 0, length_max);
    uint32_t timeout = 500000;

    while (timeout > 0) {

        if (usart1_read_buffer(&c)) {
            if (c == '$') break;
        }
        timeout--;
    }

    // Se abbiamo esaurito il tempo senza trovare '$', usciamo subito
    if (timeout == 0) {
        buffer[0] = '\0';
        return;
    }
    buffer[0] = '$';
    index = 1;
    timeout = 500000;

    while (index < (length_max - 1) && timeout > 0) {
        if (usart1_read_buffer(&c)) {
            if (c == '\n' || c == '\r') break;
            buffer[index] = c;
            index++;
            timeout = 500000;
        } else {
            timeout--;
        }
    }

    buffer[index] = '\0';
}

void NEO6M_format_gps_data(char *buffer, GPS_Data *data){
    NEO6M_read_gps_string(buffer, 100);
    if (buffer[0] == '\0') return;
    if (strncmp(buffer, "$GNGGA", 6)==0 || strncmp(buffer, "$GPGGA", 6) == 0){
        char *f;

        // time
        f = get_field(buffer, 1);
        if(f && *f != ',' && *f != '\0'){
            uint32_t time = atof(f);
            data->second = time % 100;
            data->minute = (time / 100)%100;
            data->hour = (time/10000);
        }

        // latitudine
        f = get_field(buffer, 2);
        if (f && *f != ',' && *f != '\0') {
            float val = atof(f);
            val = val/100;
            data->latitude = val;
        }

        // longitudine
        f = get_field(buffer, 4);
        if (f && *f != ',' && *f != '\0') {
                float val = atof(f);
                val = val/100;
                data->longitude = val;
            }
        // fix quality
        f = get_field(buffer, 6);
        if (f && *f != ',' && *f != '\0') data->fixValid = atoi(f);
    }
}
