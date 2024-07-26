#include "gd32e23x.h"
#include "systick.h"
#include <string.h>
#include <stdio.h>
#include "led.h"
#include "tft.h"
#include "tft_init.h"
#include "adc.h"
#include "usart.h"
#include "timer.h"
#include "key.h"
#include "freq.h"
#include "main.h"

#include "menu.h"


volatile struct Oscilloscope oscilloscope={0};

void Init_Oscilloscope(volatile struct Oscilloscope *value);

int main(void)
{      
		
	
    uint16_t i=0;
    
    //中间值
    float median=0;
    
    //峰峰值
    float voltage=0;
	
	
    float CursorTmp=0;
		uint8_t CursorDisplay=0;
    //波形放大倍数
    float gainFactor=0;
	
		float adcValue = 0;
    //触发沿标记
    uint16_t Trigger_number=0;
    
    //初始化示波器参数
    Init_Oscilloscope(&oscilloscope);
    
    //时钟初始化
    systick_config();
		
    //屏幕初始化
    TFT_Init();
		TFT_Fill(0,0,160,128,WHITE);
		
		TFT_ShowWelcomeUI();
	
    //LED初始化
    Init_LED_GPIO();
    
    //初始化串口
    Init_USART(115200);
    
    //ADC DMA初始化
    ADC_DMA_Init();
    
    //初始化ADC引脚
    Init_ADC();
    
    //初始化PWM输出
    Init_PWM_Output(oscilloscope.timerPeriod-1,oscilloscope.pwmOut);
    
		ADC_TRIG_TIMMER_Init();
		
    //初始化EC11引脚
    Init_EC11_GPIO();
    
    //初始化按键引脚
    Init_Key_GPIO();
    
    //初始化频率定时器2
    Init_FreqTimer();
		//waiting for 3s
    delay_ms(3000);
		
		TFT_ClearScreen();
    //初始化静态UI
    TFT_StaticUI();
    while(1)
    {  
        //按键扫描处理函数
				Key_Sacnf(&oscilloscope);
        Key_Handle(&oscilloscope);
        
        //如果获取电压值完成，开始刷屏
        if(oscilloscope.showbit==1)
        {           
            oscilloscope.showbit=0;
					
					  oscilloscope.pvpp=0;
					  oscilloscope.nvpp=0;
					
						Opt_ADC_Value();
            //转换电压值
            for(i=0;i<300;i++)
            {
								adcValue = (Get_ADC_Value(i)*3.3f)/4096.0f;//0-3.3
							
								oscilloscope.voltageValue[i] = (5-(2.0f*adcValue))* (oscilloscope.dimmerMultpile+0.0f);//true voltage
							

                if((oscilloscope.pvpp) < oscilloscope.voltageValue[i])//find max voltage
                {
                    oscilloscope.pvpp = oscilloscope.voltageValue[i];
                }
								if((oscilloscope.nvpp) > oscilloscope.voltageValue[i])//find max voltage
                {
                    oscilloscope.nvpp = oscilloscope.voltageValue[i];
                }
                
            }

						
						
						
						
						
            oscilloscope.vpp=(oscilloscope.pvpp-oscilloscope.nvpp);
						if(oscilloscope.vpp <= 0.3)//ignore <0.3
            {
                    oscilloscope.gatherFreq=0;
						}
						
            //刷屏的同时获取电压值
            dma_transfer_number_config(DMA_CH0, 300);
            dma_channel_enable(DMA_CH0);
            oscilloscope.isTrig=0;
            //找到起始显示波形值
            for(i=0;i<200;i++)
            {
							if(!oscilloscope.trigMode){
                if(oscilloscope.voltageValue[i] < oscilloscope.trigV)//finding rising edge
                {
                    for(;i<200;i++)
                    {
                        if(oscilloscope.voltageValue[i] > oscilloscope.trigV)
                        {
                            Trigger_number=i;
														oscilloscope.isTrig=1;
                            break;
                        }
                    }
                    break;
                }
							}else{
								if(oscilloscope.voltageValue[i] > oscilloscope.trigV)//finding falling edge
                {
                    for(;i<200;i++)
                    {
                        if(oscilloscope.voltageValue[i] < oscilloscope.trigV)
                        {
                            Trigger_number=i;
														oscilloscope.isTrig=1;
                            break;
                        }
                    }
                    break;
                }
							}
            }	
      			//获取中间值
						median = oscilloscope.vpp / 2.0f+oscilloscope.nvpp;
            //如果幅值过小，会出现放大倍数过大导致波形显示异常的问题
            if(oscilloscope.vpp > 0.3)
            {
                //放大倍数
                gainFactor = 40.0f/oscilloscope.vpp;
              
            }
						
            CursorTmp=(*oscilloscope.CursorData-median)*gainFactor;
						CursorDisplay=(CursorTmp<=50)&&(CursorTmp>=-50)&&oscilloscope.CursorShow;
            //依次显示后续100个数据，这样可以防止波形滚动
            for(i=Trigger_number;i<Trigger_number+100;i++)
            {

                voltage=oscilloscope.voltageValue[i];

                voltage = (voltage-median)*gainFactor;
							
								if(oscilloscope.pause==1)break;
							
                drawCurve(55,voltage,CursorTmp,CursorDisplay);
            }  
						
						 						
        }
				
				if(oscilloscope.pause==1){
					Open_LED(2);
					CLose_LED(1);
				}else{
					Open_LED(1);
					CLose_LED(2);
				}
				
				printf("frequence:%f\n",oscilloscope.gatherFreq);
				
        //参数显示UI
        TFT_ShowUI(&oscilloscope); 
        drawMenu(&oscilloscope); 
				drawItem(&oscilloscope); 
				resetMenuFlag(&oscilloscope);
    }
}

/*
*   函数内容：初始化示波器参数结构体
*   函数参数：volatile struct Oscilloscope *value--示波器参数结构体指针
*   返回值：无
*/
void Init_Oscilloscope(volatile struct Oscilloscope *value)
{
    (*value).showbit    =0;                         //清除显示标志位
		(*value).pause    =0; 
	  (*value).trigMode =0;
		(*value).menuSel    =0;   //0->trigger
		(*value).itemSel    =0; 	
	  (*value).dimmerMultpile =1; 
		(*value).isSel       =0; 	//0->default no selection
	  (*value).sampletime = 9;
	  (*value).adcMode=ADC_SAMPLETIME_55POINT5;
    (*value).keyValue   =0;                         //清楚按键值
    (*value).ouptputbit =0;                         //输出标志位
    (*value).gatherFreq =0;                         //采集频率
    (*value).outputFreq =1000;                      //输出频率
    (*value).pwmOut     =500;                       //PWM引脚输出的PWM占空比
    (*value).timerPeriod=1000;                      //PWM输出定时器周期
    (*value).vpp        =0.0f; 
		(*value).pvpp        =0.0f; 
		(*value).nvpp        =0.0f; 	
		(*value).trigV        =1.0f; 	
		(*value).Cursor = 0.0f;
		(*value).CursorShow = 0;
		(*value).CursorData = &((*value).trigV);
		(*value).isTrig=0;
}

