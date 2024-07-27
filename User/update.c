#include "update.h"
#include <stdlib.h>
#include <string.h>
#include "stdint.h"



int compare(const void *a, const void *b) {
    return (*(uint16_t*)a - *(uint16_t*)b);
}


void medianFilter(uint16_t *input, uint16_t *output, int length, int windowSize) {
		if(windowSize==0){
		    for (int i = 0; i < length; i++) {
					output[i] = input[i];
				}
				return;
	}
	
    int halfWindow = windowSize / 2;
    //uint16_t window[medianFilter_WindowSize];
		uint16_t *window = (uint16_t *)malloc(windowSize * sizeof(uint16_t));

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
		free(window);
}

void avgFilterLazy_FLOAT(float input, float *window, uint8_t *count, float *result, uint8_t windowSize) {
	(window)[(*count)] = input;
	(*count) = ((*count) + 1) % windowSize;
	if ((*count) == 0) {
		float totalDiff = 0;
		for (int i = 0; i < windowSize; i++) {
			totalDiff += (window)[i];			
   }
   (*result) = totalDiff / (windowSize+0.0f);        
   }
			
}

void avgFilterLazy(uint32_t input, uint32_t *window, uint8_t *count, uint32_t *result, uint8_t windowSize) {
	(window)[(*count)] = input;
	(*count) = ((*count) + 1) % windowSize;
	if ((*count) == 0) {
		uint32_t totalDiff = 0;
		for (int i = 0; i < windowSize; i++) {
			totalDiff += (window)[i];			
   }
   (*result) = totalDiff / windowSize;        
   }
			
}
void avgFilter(uint32_t input, uint32_t *window, uint8_t *count, uint32_t *result, uint8_t windowSize) {

    window[*count] = input;
    *count = (*count + 1) % windowSize;
    uint32_t total = 0;
    uint8_t validCount = (*count == 0) ? windowSize : *count;
    for (int i = 0; i < validCount; i++) {
        total += window[i];
    }
    *result = total / validCount;
}


void avgFilterArray(uint16_t *input, uint16_t *output, int length, int windowSize) {
	if(windowSize==0){
		    for (int i = 0; i < length; i++) {
					output[i] = input[i];
				}
				return;
	}
    uint32_t *window = (uint32_t *)malloc(windowSize * sizeof(uint32_t));
    //memset(window, 0, windowSize * sizeof(uint32_t));
    uint8_t count = 0;
    uint32_t result = 0;
    for (int i = 0; i < length; i++) {
			for(int w = 0; w < windowSize; w++){
				if(i+w>=length) break;
					avgFilter((uint32_t)input[i+w], window, &count, &result, (uint8_t)windowSize);
					output[i] = (uint16_t)result;
				}
    }
    free(window);
}