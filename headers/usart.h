#ifndef NEO6M
#define NEO6M

#include <stdint.h>
#include "main.h"

#define USART1 0x40013800UL

#define USART_SR (*(volatile uint32_t*)(USART1 + 0x00UL))
#define USART_DR (*(volatile uint32_t*)(USART1 + 0x04UL))
#define USART_BRR (*(volatile uint32_t*)(USART1 + 0x08UL))
#define USART_CR1 (*(volatile uint32_t*)(USART1 + 0x0CUL))
#define USART_CR2 (*(volatile uint32_t*)(USART1 + 0x10UL))

#define TXE (1U<<7) //1 se trasmissione completa
#define RXNE (1U<<5) // 1 se registro dati da leggere

#define CR1_USARTEN (1U<<13)
#define CR1_PARITYEN (1U<<10)

#define GPIOA_BASE 0x40010800UL
#define RCC_BASE 0x40021000UL

#define GPIOA_CRH (*(volatile uint32_t*)(GPIOA_BASE + 0x04UL))

#define RCC_APB2RST (*(volatile uint32_t*)(RCC_BASE + 0x0CUL))

typedef struct{
    double latitude;
    double longitude;
    int hour, minute, second;
    char fixValid;
} GPS_Data;


void usart1_init(uint8_t fclk, uint32_t baudrate);
void usart1_send(uint8_t data);
void usart1_send_string(char* data);
uint8_t usart1_receive();
void read_gps_string(char*buffer, uint32_t length_max);
void format_gps_data(char *buffer, GPS_Data *data);

#endif
