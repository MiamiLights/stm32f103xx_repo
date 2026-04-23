#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>


void SSD1306_power_on(void);
void SSD1306_write_data(char* data_array, uint16_t length);
void SSD1306_clear(void);
void draw_pixel(uint8_t x, uint8_t y, uint8_t status, char* frame_buffer);
void SSD1306_print_char_scaled(char c, uint8_t start_x, uint8_t start_y, uint8_t scale, char* buffer);
void SSD1306_print_string(char* string, uint8_t x, uint8_t y, uint8_t scale, char* buffer);

#endif
