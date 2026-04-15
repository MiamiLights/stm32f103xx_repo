#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

// LED PB2
#define LED_PIN (1U << 2)

void LED_init(void);
void LED_toggle(void);

#endif
