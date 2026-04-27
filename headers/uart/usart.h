#ifndef USART_H
#define USART_H

#include <stdint.h>

void usart1_init(uint8_t fclk, uint32_t baudrate);
void usart1_send(uint8_t data);
void usart1_send_string(char* data);
uint8_t usart1_read_buffer(uint8_t *data_out);
void usart1_flush(void);

#endif
