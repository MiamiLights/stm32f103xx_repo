#include "i2c.h"
#include "font.h"
#include "SSD1306.h"
#include <stdint.h>
#include <sys/_intsup.h>
// Command table pagina 28


void SSD1306_power_on(){
    i2c1_init();
    // Charge Pump Regulator
    i2c1_start(0x3C, 0);
    i2c1_write(0x00); // Control byte

    i2c1_write(0xAE); // display off per sicurezza
    i2c1_write(0x8D); // setup charge pump
    i2c1_write(0x14); // enable charge pump
    i2c1_write(0xAF); // display ON
    i2c1_write(0xA4); // entire display on
    i2c1_stop();
    SSD1306_clear();
}


void SSD1306_write_data(char* data_array, uint16_t length){
    i2c1_start(0x3C, 0);
    i2c1_write(0x40);
    for (uint16_t i = 0; i< length; i++) {
        i2c1_write(data_array[i]);
    }
    i2c1_stop();
}

void SSD1306_clear(){
    i2c1_start(0x3C, 0);
    i2c1_write(0x00);

    i2c1_write(0x20); // set memory addressing mode
    i2c1_write(0x00); // set horizontal memory addressing

    i2c1_write(0x22); // set page address
    i2c1_write(0x00); // page start 0
    i2c1_write(0x07); // page end 7

    i2c1_write(0x21); // set column address
    i2c1_write(0x00); // start column 0
    i2c1_write(0x7F); // end column 128
    i2c1_stop();


    i2c1_start(0x3C, 0);
    i2c1_write(0x40);
    for (uint16_t i = 0; i < 1024; i++) {
            i2c1_write(0x00);
    }
    i2c1_stop();
}

void SSD1306_print_string(char* string, uint8_t x, uint8_t y, uint8_t scale, char* buffer){
    uint8_t cursor_x = x;
    while (*string) {
        SSD1306_print_char_scaled(*string, cursor_x, y , scale, buffer);
        cursor_x += (6*scale);
        string++;
    }
}

void SSD1306_print_char_scaled(char c, uint8_t start_x, uint8_t start_y, uint8_t scale, char* buffer) {
    // Se il carattere non è valido, esci
    if (c < 32 || c > 126) return;

    // Trova l'indice della lettera nell'array Font5x7
    uint16_t index = (c - 32) * 5;
    //prendiamo le colonne corrispontendi dentro la matrice dei font
    for(uint8_t col = 0; col < 5; col++){
        uint8_t vert_number = Font5x7[index+col];
        // analizziamo riga per riga
        for(uint8_t row = 0; row<8; row++){
            //pixel acceso?
            if (vert_number & (1U<<row)) {

                // invece di disegnare un solo pixel si disegna un quadrato grande quanto
                // scale (es. 2x2, 3x3..)
                for(uint8_t scale_x = 0; scale_x < scale; scale_x++){
                    for (uint8_t scale_y = 0; scale_y < scale; scale_y++) {
                        //calcola le vere coordinate (x, y) sullo schermo
                        uint8_t pixel_x = start_x + (col*scale)+scale_x;
                        uint8_t pixel_y = start_y + (row * scale)+scale_y;

                        draw_pixel(pixel_x, pixel_y, 1, buffer);
                    }
                }
            }
        }
    }
}

// Funzione per accendere o spegnere un singolo pixel
void draw_pixel(uint8_t x, uint8_t y, uint8_t status, char* frame_buffer) {
    if (x >= 128 || y >= 64) {
        return;
    }

    // 2. TROVA IL BYTE
    // y / 8 calcola in quale Pagina (0-7) ci troviamo.
    // Moltiplicando la Pagina per 128 (larghezza) e sommando la X (colonna),
    // troviamo l'esatto byte nel nostro array 1D.
    uint16_t index = x + ((y / 8) * 128);

    // 3. TROVA IL BIT
    // Il resto della divisione (y % 8) ci dice esattamente quale
    // degli 8 pixel verticali dentro quel byte dobbiamo accendere.
    uint8_t bit = y % 8;

    // 4. MODIFICA IL PIXEL CON OPERATORI BITWISE
    if (status == 1) {
        // Accendi il pixel (Colore = 1)
        // L'operatore OR (|) lascia intatti gli altri bit e mette a '1' solo il nostro
        frame_buffer[index] |= (1 << bit);
    } else {
        // Spegni il pixel (Colore = 0)
        // L'operatore AND (&) combinato con la NOT (~) fa l'esatto opposto:
        // mette a '0' il nostro bit e lascia intatti gli altri.
        frame_buffer[index] &= ~(1 << bit);
    }
}
