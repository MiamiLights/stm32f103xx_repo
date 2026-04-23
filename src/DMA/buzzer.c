#include "common.h"
#include <stdint.h>
#include "megalovania.h"

/* Questa implementazione si basa sulla trasformazione dei campioni, di base questi non sono adatti,
 * la ragione immagino sia il volume e i valori dentro il vettore oscillano tutti più o meno intorno a 128,
 *
 *
*/


#define BUF_SIZE 512
#define HALF_BUF (BUF_SIZE/2)

uint8_t audio_ram[BUF_SIZE];

static uint32_t src_index = 0;

void fill_buffer(uint8_t* dst){
    for (int i = 0; i < HALF_BUF; i++) {
        int sample = (int)mg_raw[src_index] - 128;
        sample = sample * 5;
        // convertiamo tutto nel range 0, 255
        if (sample > 127) sample = 127;
        if(sample<-128) sample = - 128;
        dst[i] = (uint8_t)(sample+128);
        src_index++;
        if(src_index >= mg_raw_len) src_index = 0;
    }
}

void TIM3_setup(){
    fill_buffer(&audio_ram[0]);
    fill_buffer(&audio_ram[HALF_BUF]);

    RCC_APB2ENR |= (1U << 0) | (1U << 2); // AFIO + GPIOA
    RCC_APB1ENR |= (1U << 1);             // TIM3
    RCC_AHBENR  |= (1U << 0);             // DMA1

    // PA7 - Alternate Function Push-Pull
    GPIOA_CRL &= ~(0b1111U << 28);
    GPIOA_CRL |= (0b1011U << 28);

    // Configurazione Timer: ~8kHz (64MHz / 32 / 250)
    TIM3_PSC = 31-1;
    TIM3_ARR = 256-1;

    // PWM Mode 1, Enable Preload
    TIM3_CCMR1 &= ~(0b111U << 12);
    TIM3_CCMR1 |= (0b110U << 12) | (1U << 11);

    TIM3_CR1 |= (1U << 7);  // Auto reload preload enable (arr register buffered)
    TIM3_CCER |= (1U << 4); // Enable output on channel 2

    // Configurazione DMA1 Canale 3
    DMA1_CPAR(3) = (uint32_t)(&TIM3_CCR2);
    DMA1_CMAR(3) = (uint32_t)audio_ram;
    DMA1_CNDTR(3) = BUF_SIZE;

    DMA1_CCR(3) =
        (1U << 4) // read from memory
        | (1U << 5) // circular mode enabled
        | (1U << 7) // memory increment enabled
        | (1U << 8) // peripheral size = 16
        | (1U << 2) //half transfer interrupt enable
        | (1U << 1); // transfer complete interrupt enable

    DMA1_IFCR = (0xFU << 8);  // Pulisce i flag
    DMA1_CCR(3) |= (1U << 0); // Attiva DMA

    // abilitiamo gli interrupt DMA1 Ch3 nel NVIC
    // IRQ 13 = DMA1_Channel3

    NVIC_ISER0 |= (1U<<13); // bit 13 corrispondente a DMA1_CH3

    TIM3_EGR |= (1U << 0);
    TIM3_DIER |= (1U << 8); // Update DMA request enable
    TIM3_CR1 |= (1U << 0);
}

void DMA1_Channel3_IRQHandler(void){
    uint32_t isr = DMA1_ISR;

    if(isr & (1U<<10)) { // HTIF3 flag, metà trasferimento avvenuto
        DMA1_IFCR = (1U<<10); // Puliamo TUTTI i flag HTIF3
        fill_buffer(&audio_ram[0]);
    } else if(isr & (1U<<9)){ // TCIF3 flag, trasferimento completo
        DMA1_IFCR = (1U<<9); // Puliamo flag TCIF3
        fill_buffer(&audio_ram[HALF_BUF]);
    }
}
