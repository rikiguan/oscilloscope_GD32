#include "menu.h"
#include "timer.h"
#include "key.h"
#include "tft.h"
#include "PWMMeasure.h"

#define MenuX1Position 110
#define MenuX2Position 160

#define ItemX1Position 110
#define ItemX2Position 160

MENU_OptionTypeDef MENU_OptionList[] ={
{"MAIN",{"Trig","Mode","Base"},MENU_DISPLAY_MAIN,MENU_HANDLER_MAIN},
{"TOOL",{"Vmax","Cursor","Vmin"},MENU_DISPLAY_TOOL,MENU_HANDLER_TOOL},
{"PWM",{"Freq","Open","Duty"},MENU_DISPLAY_PWM,MENU_HANDLER_PWM},
{"SET",{"Dim","Adc","Filter"},MENU_DISPLAY_SET,MENU_HANDLER_SET},
{"FILTER",{"Avg","Med","(PWM)"},MENU_DISPLAY_FILTER,MENU_HANDLER_FILTER}
};
static char _MENU_showData[32]={0};
uint8_t _isChange=0;
uint8_t menuSize=ARRAY_SIZE(MENU_OptionList);
uint8_t menuVisableSize=4;

uint8_t _menuSel=99;
uint8_t _itemSel=99;
uint8_t _isSel;



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


//--------------------------FILTER-------------------------
extern uint8_t PWMMearueResult;;
char* MENU_DISPLAY_FILTER(volatile struct Oscilloscope *value,uint8_t item){
	switch(item){
			case 0:		
				sprintf(_MENU_showData,"%d",(*value).AvgFilter);		
				return (char *)_MENU_showData;
			case 1:
				sprintf(_MENU_showData,"%d",(*value).MedFilter);		
				return (char *)_MENU_showData;
			case 2:
				
				sprintf(_MENU_showData,"%d%%",PWMMearueResult);		
				return (char *)_MENU_showData;
			default: return "xxx";
		}
}


void MENU_HANDLER_FILTER(volatile struct Oscilloscope *value,uint8_t key){
			if((*value).isSel){
				if(key == KEYDPRESS){
					(*value).isSel=0;
				}
			switch((*value).itemSel){
				case 0:
					switch(key){
						  case KEYAPRESS:
								(*value).AvgFilter+=1;
								break;
						  case KEYBPRESS:
								(*value).AvgFilter=(*value).AvgFilter?((*value).AvgFilter-1):0;
								break;

					}
					break;
				case 1:
					(*value).CursorData=&((*value).Cursor);
					switch(key){
						  case KEYAPRESS:
								(*value).MedFilter+=1;
								break;
						  case KEYBPRESS:
								(*value).MedFilter=(*value).MedFilter?((*value).MedFilter-1):0;
								break;

					}
					break;
				case 2:
					TFT_ShowString(1,55,(uint8_t *)"Measuring...",WHITE,BLACK,16,0);
					PWMAnylzing(value);
					break;				
		}
			_isChange=1;
	}
}


//--------------------------FILTER--------------------------





//--------------------------TOOL-------------------------

char* MENU_DISPLAY_TOOL(volatile struct Oscilloscope *value,uint8_t item){
	switch(item){
			case 0:		
				sprintf(_MENU_showData,"%1.2fV",(*value).pvpp);		
				return (char *)_MENU_showData;
			case 1:
				sprintf(_MENU_showData,"%1.2fV",(*value).Cursor);		
				return (char *)_MENU_showData;
			case 2:
				sprintf(_MENU_showData,"%1.2fV",(*value).nvpp);		
				return (char *)_MENU_showData;
			default: return "xxx";
		}
}


void MENU_HANDLER_TOOL(volatile struct Oscilloscope *value,uint8_t key){
			if((*value).isSel){
				if(key == KEYDPRESS){
					(*value).isSel=0;
				}
			switch((*value).itemSel){
				case 0:
					
					break;
				case 1:
					(*value).CursorData=&((*value).Cursor);
					switch(key){
						  case KEYAPRESS:
								(*value).Cursor+=0.1;
								break;
						  case KEYBPRESS:
								(*value).Cursor-=0.1;
								break;

					}
					break;
				case 2:
					
					break;				
		}
			_isChange=1;
	}
}


//--------------------------TOOL--------------------------





//--------------------------SET-------------------------

char* MENU_DISPLAY_SET(volatile struct Oscilloscope *value,uint8_t item){
	switch(item){
			case 0:		
				return ((*value).dimmerMultpile==1)?"x1":"x50";
			case 1:
				sprintf(_MENU_showData,"%d",(*value).adcMode);		
				return (char *)_MENU_showData;

			case 2:
				return "ENTER";
			default: return "xxx";
		}
}


void MENU_HANDLER_SET(volatile struct Oscilloscope *value,uint8_t key){
			if((*value).isSel){
				if(key == KEYDPRESS){
					(*value).isSel=0;
				}
			switch((*value).itemSel){
				case 0:
					if(key & 0b00110000){
						if((*value).dimmerMultpile == 1)
            {
                (*value).dimmerMultpile=50;
            }
            else
            {
                (*value).dimmerMultpile=1;
            }
					}
					break;
				case 1:
					switch(key){
						  case KEYAPRESS:
								(*value).adcMode=((*value).adcMode+1)%8;
								break;
						  case KEYBPRESS:
								(*value).adcMode=((*value).adcMode-1)%8;
								break;
					}
					
					 adc_regular_channel_config(0, ADC_CHANNEL_3, (*value).adcMode);
					break;
				case 2:
						(*value).menuSel=4;
						(*value).isSel=0;
						(*value).itemSel=0;
					break;				
		}
			_isChange=1;
	}
}


//--------------------------SET--------------------------



//--------------------------PWM-------------------------

char* MENU_DISPLAY_PWM(volatile struct Oscilloscope *value,uint8_t item){
	switch(item){
			case 0:
					sprintf(_MENU_showData,"%d",(*value).outputFreq);		
					return (char *)_MENU_showData;
			case 1:
				if((*value).ouptputbit){
					return "OPEN";
				}else{
					return "CLOSE";
				}
			case 2:
					sprintf(_MENU_showData,"%2d%%",(uint16_t)((((*value).pwmOut)/((*value).timerPeriod+0.0f))*100));
					return (char *)_MENU_showData;
					
			default: return "xxx";
		}
}


void MENU_HANDLER_PWM(volatile struct Oscilloscope *value,uint8_t key){
       float tempValue=0;
			if((*value).isSel){
				if(key == KEYDPRESS){
					(*value).isSel=0;
				}
			switch((*value).itemSel){
				case 0:
					tempValue=(*value).pwmOut/((*value).timerPeriod+0.0f);
					switch(key){
						  case KEYAPRESS:
								(*value).outputFreq+=100;
								break;
						  case KEYBPRESS:
								(*value).outputFreq-=100;
								break;
					}   
//            if((*value).timerPeriod < 250)
//            {
//                (*value).timerPeriod = 1000;
//            }
						
					  (*value).timerPeriod = 1000000/(*value).outputFreq;
						(*value).pwmOut = (uint16_t)((*value).timerPeriod*tempValue);
						Set_Output_PWMComparex((*value).pwmOut);
            Set_Output_Freq((*value).timerPeriod-1);
					break;
				case 1:
					if(key & 0b00110000){
						if((*value).ouptputbit == 0)
            {
                (*value).ouptputbit=1;
                timer_enable(TIMER14);
            }
            else
            {
                (*value).ouptputbit=0;
               timer_disable(TIMER14); 
            }
					}
					break;
				case 2:
					switch(key){
						  case KEYAPRESS:
								(*value).pwmOut = ((uint32_t)(*value).timerPeriod*0.01f)+(*value).pwmOut;
								break;
						  case KEYBPRESS:
								(*value).pwmOut = -((uint32_t)(*value).timerPeriod*0.01f)+(*value).pwmOut;
								break;
					}
            if((*value).pwmOut > (*value).timerPeriod)
            {
                (*value).pwmOut = 0;
            }
            Set_Output_PWMComparex((*value).pwmOut);
					break;				
		}
			_isChange=1;
	}
}


//--------------------------PWM--------------------------



//--------------------------MAIN-------------------------

char* MENU_DISPLAY_MAIN(volatile struct Oscilloscope *value,uint8_t item){
		switch(item){
			case 0:
					sprintf(_MENU_showData,"%1.2fV",(*value).trigV);
					return (char *)_MENU_showData;
			case 1:
				if((*value).trigMode){
					return "FALL";
				}else{
					return "RISE";
				}
			case 2:
				sprintf(_MENU_showData,"%dus",((*value).sampletime+1)*10);
				return (char *)_MENU_showData;
					
			default: return "xxx";
		}
}

void MENU_HANDLER_MAIN(volatile struct Oscilloscope *value,uint8_t key){
		if((*value).isSel){
			if(key == KEYDPRESS){
				(*value).isSel=0;
			}
			switch((*value).itemSel){
				case 0:
					
					(*value).CursorData=&((*value).trigV);
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
					if(key & 0b00110000)
					(*value).trigMode=!(*value).trigMode;
					break;
				case 2:
					switch(key){
						  case KEYAPRESS:
								(*value).sampletime+=1;
								break;
						  case KEYBPRESS:
								(*value).sampletime-=1;
								break;
					}
					timer_autoreload_value_config(TIMER0,(*value).sampletime);
					break;			
		}
			_isChange=1;
	}
	
}
//-------------------------MAIN----------------------------

void drawMenu(volatile struct Oscilloscope *value){
	if((_menuSel!=(*value).menuSel)){
	TFT_Fill(MenuX1Position,0,MenuX2Position,16,YELLOW);
	TFT_Fill(MenuX1Position,20,MenuX2Position,32,PURPLE);
	TFT_Fill(MenuX1Position,56,MenuX2Position,68,PURPLE);
	TFT_Fill(MenuX1Position,92,MenuX2Position,104,PURPLE);
	TFT_ShowString(110,0,(uint8_t *)MENU_OptionList[(*value).menuSel].String,BLACK,YELLOW,16,0);
	TFT_ShowString(110,20,(uint8_t *)MENU_OptionList[(*value).menuSel].item[0],WHITE,PURPLE,12,0);
	TFT_ShowString(110,56,(uint8_t *)MENU_OptionList[(*value).menuSel].item[1],WHITE,PURPLE,12,0);
	TFT_ShowString(110,92,(uint8_t *)MENU_OptionList[(*value).menuSel].item[2],WHITE,PURPLE,12,0);
	
	}
}


void drawItem(volatile struct Oscilloscope *value){
	if((_menuSel!=(*value).menuSel)||(_itemSel!=(*value).itemSel)||(_isSel!=(*value).isSel)||	_isChange||(_menuSel!=(*value).menuSel)){
	TFT_Fill(ItemX1Position,36,ItemX2Position,52,GRAY);
	TFT_Fill(ItemX1Position,72,ItemX2Position,88,GRAY);
	TFT_Fill(ItemX1Position,108,ItemX2Position,124,GRAY);
	TFT_ShowString(110,36,(uint8_t *)MENU_OptionList[(*value).menuSel].funcdis(value,0),(((*value).itemSel==0)&&((*value).isSel==1))?PURPLE:BLACK,YELLOW,((*value).itemSel==0)?16:12,0);
	TFT_ShowString(110,72,(uint8_t *)MENU_OptionList[(*value).menuSel].funcdis(value,1),(((*value).itemSel==1)&&((*value).isSel==1))?PURPLE:BLACK,YELLOW,((*value).itemSel==1)?16:12,0);
	TFT_ShowString(110,108,(uint8_t *)MENU_OptionList[(*value).menuSel].funcdis(value,2),(((*value).itemSel==2)&&((*value).isSel==1))?PURPLE:BLACK,YELLOW,((*value).itemSel==2)?16:12,0);
	
	}
}
void resetMenuFlag(volatile struct Oscilloscope *value){
	_menuSel=(*value).menuSel;
	_itemSel=(*value).itemSel;
	_isSel=(*value).isSel;
	_isChange=0;
	_menuSel=(*value).menuSel;
}
