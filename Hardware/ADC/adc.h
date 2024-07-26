#ifndef __ADC_H
#define __ADC_H

#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>

uint16_t Get_ADC_Value(uint16_t value);
void Init_ADC(void);
void Opt_ADC_Value();
void ADC_DMA_Init(void);
void ADC_Config(void);
unsigned int Get_ADC_Value2(uint8_t  ADC_CHANNEL_x);
void ADC_TRIG_TIMMER_Init();
#endif