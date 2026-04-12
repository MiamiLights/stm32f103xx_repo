#ifndef TIMEOUT_H_
#define TIMEOUT_H_

#include <stdint.h>
#include "common.h"

typedef enum
{
	OK=0,
	Error=1
} TypdedefStatus;

#define CSR_COUNTFLAG (1U<<16)

uint32_t ticks_init(uint32_t freq);
void delay(uint32_t delay_ms);

#endif
