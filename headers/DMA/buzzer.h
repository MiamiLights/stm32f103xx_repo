#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

void TIM3_setup(void);
void TIM3_test_static(void);
void elabora_audio(uint32_t start_index);
void TIM3_update_buffer(void);

#endif
