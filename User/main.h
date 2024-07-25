#ifndef MAIN_H
#define MAIN_H

struct Oscilloscope
{
   uint8_t showbit;         //画面更新标志位
	 uint8_t pause;
   uint8_t keyValue;        //按键值
   uint8_t ouptputbit;      //输出标志位
   uint16_t outputFreq;     //波形输出频率
   uint16_t pwmOut;         //PWM引脚输出的PWM占空比
   uint32_t sampletime;     //电压采集时间
   uint32_t timerPeriod;    //定时器周期设置
   uint32_t gatherFreq;        //示波器采集频率
	 float pvpp;
	 float nvpp; 	
   float vpp;               //峰峰值
   float voltageValue[300]; //ADC采集电压值
};

#endif /* MAIN_H */
