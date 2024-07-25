#ifndef MAIN_H
#define MAIN_H
#include "stdint.h"

struct Oscilloscope
{
	
	 uint8_t showbit;         //������±�־λ
	 uint8_t pause;
	 
	 float trigV;
	 uint8_t trigMode;
	
   uint8_t keyValue;        //����ֵ
	 uint8_t menuSel;
	 uint8_t itemSel;
	 uint8_t isSel;
	
   uint8_t ouptputbit;      //�����־λ
   uint16_t outputFreq;     //�������Ƶ��
   uint16_t pwmOut;         //PWM���������PWMռ�ձ�
   uint32_t sampletime;     //��ѹ�ɼ�ʱ��
   uint32_t timerPeriod;    //��ʱ����������
   float gatherFreq;        //ʾ�����ɼ�Ƶ��
	 float pvpp;
	 float nvpp; 	
   float vpp;               //���ֵ
   float voltageValue[300]; //ADC�ɼ���ѹֵ
	
  
};


typedef struct _MENU_OptionTypeDef
{
    char *String;
		char* item[3];
    char *(*funcdis)(volatile struct Oscilloscope *value,uint8_t item);
		void (*funchand)(volatile struct Oscilloscope *value,uint8_t key); 	
} MENU_OptionTypeDef;

void MENU_SEL_HANDLER(volatile struct Oscilloscope *value,uint8_t key);

#endif /* MAIN_H */
