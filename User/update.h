#ifndef UPDATE_H
#define UPDATE_H

#include "stdint.h"
void medianFilter(uint16_t *input, uint16_t *output, int length, int windowSize);
void avgFilter(uint32_t input, uint32_t *window, uint8_t *count, uint32_t *result, uint8_t windowSize);
void avgFilterLazy(uint32_t input, uint32_t *window, uint8_t *count, uint32_t *result, uint8_t windowSize);
void avgFilterArray(uint16_t *input, uint16_t *output, int length, int windowSize);
void avgFilterLazy_FLOAT(float input, float *window, uint8_t *count, float *result, uint8_t windowSize);
#endif