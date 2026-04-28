#include "spi.h"
#include "common.h"
#include "timeout.h"
#include <stdint.h>

// PA4 CS

#define CS_LOW_SET()  GPIOA_BSRR = (1U<<20)
#define CS_HIGH_SET() GPIOA_BSRR = (1U<<4)


/*
 * startup:
 * 1. clock tra 100 e 400khz.
 * 2. DI e CS ad alto;
 * 3. applica 74 o più pulsi di clock
 */

uint8_t sd_send_command(uint8_t command, uint32_t arg, uint8_t crc ){
    uint8_t res;

    // inviamo l'indice del comando (01-000000)
    SPI1_transmit(0x40 | command);

    // inviamo l'argomento (32 bits), spacchiamolo in 4 byte e inviamo prima il più significativo
    SPI1_transmit((uint8_t)(arg >> 24));
    SPI1_transmit((uint8_t)(arg >> 16));
    SPI1_transmit((uint8_t)(arg >> 8));
    SPI1_transmit((uint8_t)arg);

    // invia il byte di controllo (CRC)
    SPI1_transmit(crc);

    /* Aspettiamo la risposta della scheda.
     * La scheda può impiegare fino a 8 cicli di clock per rispondere.
     * Aumentiamo i tentativi a 100 per maggiore compatibilità.
     */
    for (int i = 0; i < 10; i++) {
        res = SPI1_transmit(0xFF);
        if (res != 0xFF) break; // Trovata la risposta!
    }

    return res;
}

uint8_t sd_init(void){
    uint8_t response;
    uint32_t timeout = 2000;

    CS_HIGH_SET();
    delay(10); // Piccolo delay per stabilizzare la tensione
    for(int i = 0; i < 10; i++) SPI1_transmit(0xFF); // 80 dummy clocks

    CS_LOW_SET();

    // software reset (CMD0) - Proviamo più volte se fallisce


    if (sd_send_command(0, 0, 0x95) != 0x01){
        CS_HIGH_SET();
        return 1; // errore Reset
    }

    // controllo voltaggio
    response = sd_send_command(8, 0x000001AA, 0x87);
    /*
     * Poichè cmd8 risponde con un R7, ossia (R1 + 32 bits di coda) che a noi non interessano
     */
    SPI1_transmit(0xFF); SPI1_transmit(0xFF); SPI1_transmit(0xFF); SPI1_transmit(0xFF);

    uint8_t res55;
    do {

        res55 = sd_send_command(55, 0, 0xFF);

        // Se il CMD55 va in errore (0x05 = Illegal Command), la scheda è confusa
        if (res55 != 0x01 && res55 != 0x00) {
            CS_HIGH_SET();
            return 3; // Codice d'errore: CMD55 Rifiutato
        }

        response = sd_send_command(41, 0x40000000, 0xFF);
        timeout--;
    } while (response != 0x00 && timeout > 0);

    // finito, chiudiamo la comunicazione.

    SPI1_transmit(0xFF);
    if (timeout == 0){
        CS_HIGH_SET();
        return 2;
    }

    response = sd_send_command(16, 512, 0xFF);

    CS_HIGH_SET();
    SPI1_transmit(0xFF);

    return 0;
}

uint8_t sd_read_sector(uint32_t sector, uint8_t *buffer){
    uint8_t res;
    uint32_t timeout;

    CS_LOW_SET();
    res = sd_send_command(17, sector, 0xFF);
    if(res != 0x00) {
        CS_HIGH_SET();
        return 1; // la scheda ha rifiutato il comando (R1 response)
    }
    timeout = 500000;
    do{
        res = SPI1_transmit(0xFF);
    } while (res != 0xFE && --timeout > 0);

    if(timeout == 0){
        CS_HIGH_SET();
        return 1; // timeout, dati non arrivati
    }

    // nessun errore, possiamo leggere i dati;
    for(int i = 0; i < 512; i++){
        buffer[i] = SPI1_transmit(0xFF);
    }
    // aspiriamo e scartiamo i 2 byte di CRC finale
    SPI1_transmit(0xFF);
    SPI1_transmit(0xFF);

    CS_HIGH_SET();
    SPI1_transmit(0xFF);
    return 0;
}

uint8_t sd_write_sector(uint32_t sector, const uint8_t* buffer){
    uint8_t res;
    uint32_t timeout;

    CS_LOW_SET();

    res = sd_send_command(24, sector, 0xFF);
    if(res != 0x00){
        CS_HIGH_SET();
        SPI1_transmit(0xFF);
        return 1; // comando rifiutato
    }

    // aspettiamo 8 cicli di clock
    SPI1_transmit(0xFF);

    // inviamo il data token per avvisare la sched dell'arrivo dei dati
    SPI1_transmit(0xFE);

    // inviamo 512 byte del nostro array
    for(int i = 0; i < 512; i++){
        SPI1_transmit(buffer[i]);
    }

    // inviamo due byte finti per il CRC
    SPI1_transmit(0xFF);
    SPI1_transmit(0xFF);

    // leggiamo la data response (ci indica se i dati erano buoni)
    res = SPI1_transmit(0xFF);
    if ((res & 0b11111) != 0x05){
        CS_HIGH_SET();
        return 2; // i dati sono stati rifiutati
    }

    // Busy wait, scrivere sulla memoria flash richiede tempo, finchè la scheda starà
    // scrivendo, terrà la linea MISO a 0
    timeout = 1000000;
    do{
        res = SPI1_transmit(0xFF);
    } while(res == 0x00 && --timeout > 0);

    if(timeout == 0){
        CS_HIGH_SET();
        return 3; // timeout scrittura
    }

    CS_HIGH_SET();
    SPI1_transmit(0xFF); // rilascio del bus

    return 0;
}
