#include "update.h"
#include <stdlib.h>
#include "stdint.h"
#define WindowSize 5

int compare(const void *a, const void *b) {
    return (*(uint16_t*)a - *(uint16_t*)b);
}


void medianFilter(uint16_t *input, uint16_t *output, int length, int windowSize) {
    int halfWindow = windowSize / 2;
    uint16_t window[WindowSize];

    for (int i = 0; i < length; i++) {

        for (int j = 0; j < windowSize; j++) {
            int index = i + j - halfWindow;
            if (index < 0) index = 0;
            if (index >= length) index = length - 1;
            window[j] = input[index];
        }

        qsort(window, windowSize, sizeof(uint16_t), compare);

        output[i] = window[halfWindow];
    }
}