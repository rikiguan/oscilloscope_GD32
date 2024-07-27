#ifndef PWM_MEASURE_H
#define PWM_MEASURE_H
#include "stdint.h"
#include "main.h"
uint8_t PWMAnylzing(volatile struct Oscilloscope *value);
uint8_t PWMMeasueOnce(volatile struct Oscilloscope *value);
#endif