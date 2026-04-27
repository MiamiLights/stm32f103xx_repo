#ifndef SD_H
#define SD_H

#include <stdint.h>

uint8_t sd_send_command(uint8_t command, uint32_t arg, uint8_t crc );
uint8_t sd_init(void);
uint8_t sd_read_sector(uint32_t sector, uint8_t *buffer);
uint8_t sd_write_sector(uint32_t sector, const uint8_t* buffer);

#endif
