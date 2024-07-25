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

//TODO
#include "key.h"
    
void MENU_SEL_HANDLER(volatile struct Oscilloscope *value,uint8_t key){
		switch(key){
			case KEYAPRESS:
				if((*value).itemSel>=1)
					(*value).itemSel-=1;
				break;
		  case KEYBPRESS:
				if((*value).itemSel<=1)
					(*value).itemSel+=1;
				break;
			case KEYDPRESS:
				(*value).isSel=1;
				return;
		}
		
}
uint8_t _isChange=0;
void MENU_HANDLER_PWM(volatile struct Oscilloscope *value,uint8_t key){}
void MENU_HANDLER_MAIN(volatile struct Oscilloscope *value,uint8_t key){
		if((*value).isSel){
			if(key == KEYDPRESS){
				(*value).isSel=0;
			}
			switch((*value).itemSel){
				case 0:
					switch(key){
						  case KEYAPRESS:
								(*value).trigV+=0.1;
								break;
						  case KEYBPRESS:
								(*value).trigV-=0.1;
								break;
					}
					break;
				case 1:
					(*value).trigMode=!(*value).trigMode;
					break;
				case 2:
					(*value).pause=!(*value).pause;
					break;				
		}
	}
	_isChange=1;
}

static char _MENU_showData[32]={0};
char* MENU_DISPLAY_PWM(volatile struct Oscilloscope *value,uint8_t item){
	return "xxx";
}
char* MENU_DISPLAY_MAIN(volatile struct Oscilloscope *value,uint8_t item){
		switch(item){
			case 0:
					sprintf(_MENU_showData,"%1.2fV ",(*value).trigV);
					return (char *)_MENU_showData;
			case 1:
				if((*value).trigMode){
					return "UP";
				}else{
					return "DOWN";
				}
			case 2:
				if((*value).pause){
					return "STOP";
				}else{
					return "RUN";
				}
					
			default: return "xxx";
		}
}

MENU_OptionTypeDef MENU_OptionList[] ={
{"main",{"trig","mode","pause"},MENU_DISPLAY_MAIN,MENU_HANDLER_MAIN},
{"pwm",{"freq","open","duty"},MENU_DISPLAY_PWM,MENU_HANDLER_PWM}
};

uint8_t menuNum=2;
#define MenuX1Position 110
#define MenuX2Position 160

#define ItemX1Position 110
#define ItemX2Position 160

uint8_t _menuSel=99;
uint8_t _itemSel=99;
uint8_t _isSel;

void drawMenu(volatile struct Oscilloscope *value){
	if((_menuSel!=(*value).menuSel)){
	TFT_Fill(MenuX1Position,0,MenuX2Position,16,YELLOW);
	TFT_Fill(MenuX1Position,20,MenuX2Position,32,YELLOW);
	TFT_Fill(MenuX1Position,56,MenuX2Position,68,YELLOW);
	TFT_Fill(MenuX1Position,92,MenuX2Position,104,YELLOW);
	TFT_ShowString(110,0,(uint8_t *)MENU_OptionList[(*value).menuSel].String,BLACK,YELLOW,16,0);
	TFT_ShowString(110,20,(uint8_t *)MENU_OptionList[(*value).menuSel].item[0],WHITE,PURPLE,12,0);
	TFT_ShowString(110,56,(uint8_t *)MENU_OptionList[(*value).menuSel].item[1],WHITE,PURPLE,12,0);
	TFT_ShowString(110,92,(uint8_t *)MENU_OptionList[(*value).menuSel].item[2],WHITE,PURPLE,12,0);
	_menuSel=(*value).menuSel;
	}
}
uint8_t _menuSel;
uint8_t _itemSel;
uint8_t _isSel;

void drawItem(volatile struct Oscilloscope *value){
	if((_menuSel!=(*value).menuSel)||(_itemSel!=(*value).itemSel)||(_isSel!=(*value).isSel)||	_isChange){
	TFT_Fill(ItemX1Position,36,ItemX2Position,52,GRAY);
	TFT_Fill(ItemX1Position,72,ItemX2Position,88,GRAY);
	TFT_Fill(ItemX1Position,108,ItemX2Position,124,GRAY);
	TFT_ShowString(110,36,(uint8_t *)MENU_OptionList[(*value).menuSel].funcdis(value,0),(((*value).itemSel==0)&&((*value).isSel==1))?PURPLE:BLACK,YELLOW,((*value).itemSel==0)?16:12,0);
	TFT_ShowString(110,72,(uint8_t *)MENU_OptionList[(*value).menuSel].funcdis(value,1),(((*value).itemSel==1)&&((*value).isSel==1))?PURPLE:BLACK,YELLOW,((*value).itemSel==1)?16:12,0);
	TFT_ShowString(110,108,(uint8_t *)MENU_OptionList[(*value).menuSel].funcdis(value,2),(((*value).itemSel==2)&&((*value).isSel==1))?PURPLE:BLACK,YELLOW,((*value).itemSel==2)?16:12,0);
	_menuSel=(*value).menuSel;
	_itemSel=(*value).itemSel;
	_isSel=(*value).isSel;
	_isChange=0;
	}
}


volatile struct Oscilloscope oscilloscope={0};

void Init_Oscilloscope(volatile struct Oscilloscope *value);

int main(void)
{      
    uint16_t i=0;
    
    //中间值
    float median=0;
    
    //峰峰值
    float voltage=0;
    

    //波形放大倍数
    float gainFactor=0;
	
		float adcValue = 0;
    
    //触发沿标记
    uint16_t Trigger_number=0;
    
    //初始化示波器参数
    Init_Oscilloscope(&oscilloscope);
    
    //时钟初始化
    systick_config();
    
    //LED初始化
    Init_LED_GPIO();
    
    //屏幕初始化
    TFT_Init();
    
    //填充白色
    TFT_Fill(0,0,160,128,BLACK);
    
    //初始化串口
    Init_USART(115200);
    
    //ADC DMA初始化
    ADC_DMA_Init();
    
    //初始化ADC引脚
    Init_ADC();
    
    //初始化PWM输出
    Init_PWM_Output(oscilloscope.timerPeriod-1,oscilloscope.pwmOut);
    
    //初始化EC11引脚
    Init_EC11_GPIO();
    
    //初始化按键引脚
    Init_Key_GPIO();
    
    //初始化频率定时器2
    Init_FreqTimer();
    
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
            //转换电压值
            for(i=0;i<300;i++)
            {
								adcValue = (Get_ADC_Value(i)*3.3f)/4096.0f;//0-3.3
							
								oscilloscope.voltageValue[i] = (5-(2.0f*adcValue));//true voltage
							
                if((oscilloscope.pvpp) < oscilloscope.voltageValue[i])//find max voltage
                {
                    oscilloscope.pvpp = oscilloscope.voltageValue[i];
                }
								if((oscilloscope.nvpp) > oscilloscope.voltageValue[i])//find max voltage
                {
                    oscilloscope.nvpp = oscilloscope.voltageValue[i];
                }
                
            }
            oscilloscope.vpp=oscilloscope.pvpp-oscilloscope.nvpp;
						if(oscilloscope.vpp <= 0.3)//ignore <0.3
            {
                    oscilloscope.gatherFreq=0;
						}
						
            //刷屏的同时获取电压值
            dma_transfer_number_config(DMA_CH0, 300);
            dma_channel_enable(DMA_CH0);
            
            //找到起始显示波形值
            for(i=0;i<200;i++)
            {
                if(oscilloscope.voltageValue[i] < oscilloscope.trigV)//finding rising edge
                {
                    for(;i<200;i++)
                    {
                        if(oscilloscope.voltageValue[i] > oscilloscope.trigV)
                        {
                            Trigger_number=i;
                            break;
                        }
                    }
                    break;
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
            
            //依次显示后续100个数据，这样可以防止波形滚动
            for(i=Trigger_number;i<Trigger_number+100;i++)
            {

                voltage=oscilloscope.voltageValue[i];

                voltage = (voltage-median)*gainFactor;
							
								if(oscilloscope.pause==1)break;
							
                drawCurve(55,voltage);
            }          
        }        
        //参数显示UI
        //TFT_ShowUI(&oscilloscope); 
        drawMenu(&oscilloscope); 
				drawItem(&oscilloscope); 
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
		(*value).isSel       =0; 	//0->default no selection
    (*value).sampletime =ADC_SAMPLETIME_239POINT5;  //adc采样周期
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
}

void Init_Menu()
{
	
}
