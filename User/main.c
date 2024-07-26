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
    
    //�м�ֵ
    float median=0;
    
    //���ֵ
    float voltage=0;
	
	
    float CursorTmp=0;
		uint8_t CursorDisplay=0;
    //���ηŴ���
    float gainFactor=0;
	
		float adcValue = 0;
    //�����ر��
    uint16_t Trigger_number=0;
    
    //��ʼ��ʾ��������
    Init_Oscilloscope(&oscilloscope);
    
    //ʱ�ӳ�ʼ��
    systick_config();
		
    //��Ļ��ʼ��
    TFT_Init();
		TFT_Fill(0,0,160,128,WHITE);
		
		TFT_ShowWelcomeUI();
	
    //LED��ʼ��
    Init_LED_GPIO();
    
    //��ʼ������
    Init_USART(115200);
    
    //ADC DMA��ʼ��
    ADC_DMA_Init();
    
    //��ʼ��ADC����
    Init_ADC();
    
    //��ʼ��PWM���
    Init_PWM_Output(oscilloscope.timerPeriod-1,oscilloscope.pwmOut);
    
		ADC_TRIG_TIMMER_Init();
		
    //��ʼ��EC11����
    Init_EC11_GPIO();
    
    //��ʼ����������
    Init_Key_GPIO();
    
    //��ʼ��Ƶ�ʶ�ʱ��2
    Init_FreqTimer();
		//waiting for 3s
    delay_ms(3000);
		
		TFT_ClearScreen();
    //��ʼ����̬UI
    TFT_StaticUI();
    while(1)
    {  
        //����ɨ�账����
				Key_Sacnf(&oscilloscope);
        Key_Handle(&oscilloscope);
        
        //�����ȡ��ѹֵ��ɣ���ʼˢ��
        if(oscilloscope.showbit==1)
        {           
            oscilloscope.showbit=0;
					
					  oscilloscope.pvpp=0;
					  oscilloscope.nvpp=0;
					
						Opt_ADC_Value();
            //ת����ѹֵ
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
						
            //ˢ����ͬʱ��ȡ��ѹֵ
            dma_transfer_number_config(DMA_CH0, 300);
            dma_channel_enable(DMA_CH0);
            oscilloscope.isTrig=0;
            //�ҵ���ʼ��ʾ����ֵ
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
      			//��ȡ�м�ֵ
						median = oscilloscope.vpp / 2.0f+oscilloscope.nvpp;
            //�����ֵ��С������ַŴ��������²�����ʾ�쳣������
            if(oscilloscope.vpp > 0.3)
            {
                //�Ŵ���
                gainFactor = 40.0f/oscilloscope.vpp;
              
            }
						
            CursorTmp=(*oscilloscope.CursorData-median)*gainFactor;
						CursorDisplay=(CursorTmp<=50)&&(CursorTmp>=-50)&&oscilloscope.CursorShow;
            //������ʾ����100�����ݣ��������Է�ֹ���ι���
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
				
        //������ʾUI
        TFT_ShowUI(&oscilloscope); 
        drawMenu(&oscilloscope); 
				drawItem(&oscilloscope); 
				resetMenuFlag(&oscilloscope);
    }
}

/*
*   �������ݣ���ʼ��ʾ���������ṹ��
*   ����������volatile struct Oscilloscope *value--ʾ���������ṹ��ָ��
*   ����ֵ����
*/
void Init_Oscilloscope(volatile struct Oscilloscope *value)
{
    (*value).showbit    =0;                         //�����ʾ��־λ
		(*value).pause    =0; 
	  (*value).trigMode =0;
		(*value).menuSel    =0;   //0->trigger
		(*value).itemSel    =0; 	
	  (*value).dimmerMultpile =1; 
		(*value).isSel       =0; 	//0->default no selection
	  (*value).sampletime = 9;
	  (*value).adcMode=ADC_SAMPLETIME_55POINT5;
    (*value).keyValue   =0;                         //�������ֵ
    (*value).ouptputbit =0;                         //�����־λ
    (*value).gatherFreq =0;                         //�ɼ�Ƶ��
    (*value).outputFreq =1000;                      //���Ƶ��
    (*value).pwmOut     =500;                       //PWM���������PWMռ�ձ�
    (*value).timerPeriod=1000;                      //PWM�����ʱ������
    (*value).vpp        =0.0f; 
		(*value).pvpp        =0.0f; 
		(*value).nvpp        =0.0f; 	
		(*value).trigV        =1.0f; 	
		(*value).Cursor = 0.0f;
		(*value).CursorShow = 0;
		(*value).CursorData = &((*value).trigV);
		(*value).isTrig=0;
}

