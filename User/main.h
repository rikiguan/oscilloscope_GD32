#ifndef MAIN_H
#define MAIN_H
#include "stdint.h"

struct Oscilloscope
{
	
	 uint8_t showbit;         //画面更新标志位
	 uint8_t pause;
	 
	 float trigV;
	 uint8_t trigMode;
	
   uint8_t keyValue;        //按键值
	 uint8_t menuSel;
	 uint8_t itemSel;
	 uint8_t isSel;
	
   uint8_t ouptputbit;      //输出标志位
   uint16_t outputFreq;     //波形输出频率
   uint16_t pwmOut;         //PWM引脚输出的PWM占空比
   uint32_t sampletime;     //电压采集时间
   uint32_t timerPeriod;    //定时器周期设置
   float gatherFreq;        //示波器采集频率
	 float pvpp;
	 float nvpp; 	
   float vpp;               //峰峰值
   float voltageValue[300]; //ADC采集电压值
	
  
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
