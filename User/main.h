#ifndef MAIN_H
#define MAIN_H
#include "stdint.h"

struct Oscilloscope
{
	
	 uint8_t showbit;         //������±�־λ
	 uint8_t pause;
	 
	 float trigV;
	 float Cursor;
	 volatile float* CursorData;
	 uint16_t CursorShow;
	 uint8_t trigMode;
	 uint8_t dimmerMultpile;
   uint8_t keyValue;        //����ֵ
	 uint8_t menuSel;
	 uint8_t itemSel;
	 uint8_t isSel;
		uint8_t isTrig;
   uint8_t ouptputbit;      //�����־λ
   uint16_t outputFreq;     //�������Ƶ��
   uint16_t pwmOut;         //PWM���������PWMռ�ձ�
   uint32_t sampletime;     //��ѹ�ɼ�ʱ��
	 uint32_t adcMode;
	uint32_t timerPeriod;    //��ʱ����������

	 uint8_t AvgFilter;
	 uint8_t MedFilter;

	
   float gatherFreq;        //ʾ�����ɼ�Ƶ��
	 float pvpp;
	 float nvpp; 	
   float vpp;               //���ֵ
   float voltageValue[300]; //ADC�ɼ���ѹֵ
	
	
  
};

void Init_Oscilloscope(volatile struct Oscilloscope *value);

#endif /* MAIN_H */
