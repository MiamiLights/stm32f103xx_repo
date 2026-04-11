#ifndef TIMEOUT_H_
#define TIMEOUT_H_

#include "stdint.h"
#include <stdint.h>

typedef enum
{
	OK=0,
	Error=1
}TypdedefStatus;

#define SYSTICK_BASE 0xE000E010UL
#define SYSTICK_CSR (*(volatile uint32_t *)(SYSTICK_BASE + 0x00UL))
#define SYSTICK_RVR (*(volatile uint32_t *)(SYSTICK_BASE + 0x04UL))
#define SYSTICK_CVR (*(volatile uint32_t *)(SYSTICK_BASE + 0x08UL))
#define SYSTICK_CALIB (*(volatile uint32_t *)(SYSTICK_BASE + 0x0CUL))

#define CSR_COUNTFLAG (1U<<16)

/*
 * Il valore contenuto dentro LOAD, ossia il valore del counter può essere espresso da max 24 bits.
 * Questo valore sarà caricato dentro CVR quando il numero al suo interno arriverà a zero
 * (ATTENZIONE: deve essere abilitato il counter dentro CSR).
 */

uint32_t ticks_init(uint32_t freq);

void delay(uint32_t delay_ms);


#endif
