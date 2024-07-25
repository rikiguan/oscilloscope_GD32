#include "key.h"
#include "timer.h"
#include "tft.h"

//TODO
#include "main.h"

static uint8_t keyValue=0;
static uint8_t key1_state = 0;
static uint8_t key2_state = 0;
static uint8_t key3_state = 0;
static uint8_t key4_state = 0;
/*
*   º¯ÊýÄÚÈÝ£º³õÊ¼»¯°´¼üGPIO
*   º¯Êý²ÎÊý£ºÎÞ
*   ·µ»ØÖµ£º  ÎÞ
*/
void Init_Key_GPIO(void)
{
	//Ê¹ÄÜÊ±ÖÓ
    rcu_periph_clock_enable(RCU_GPIOB);
	
	//ÉèÖÃÊä³öÄ£Ê½£¬ÉÏÀ­
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3|GPIO_PIN_9|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

    //ÖÐ¶ÏÏßÊ¹ÄÜ
    nvic_irq_enable(EXTI4_15_IRQn,3U);
    
    //ÅäÖÃÖÐ¶ÏÏ
	  syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN9);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN13);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN14);
		syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN15);
    
    //³õÊ¼»¯ÖÐ¶ÏÏß£¬ÉèÖÃÎªÖÐ¶ÏÄ£Ê½£¬ÏÂ½µÑØ´¥·¢
		exti_init(EXTI_9,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    exti_init(EXTI_13,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    exti_init(EXTI_14,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    exti_init(EXTI_15,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    
    //ÖÐ¶Ï±êÖ¾Î»Çå³ý
		exti_interrupt_flag_clear(EXTI_9);
    exti_interrupt_flag_clear(EXTI_13);
    exti_interrupt_flag_clear(EXTI_14);
    exti_interrupt_flag_clear(EXTI_15);
}

/*
*   º¯ÊýÄÚÈÝ£º³õÊ¼»¯EC11 GPIO
*   º¯Êý²ÎÊý£ºÎÞ
*   ·µ»ØÖµ£º  ÎÞ
*/
void Init_EC11_GPIO(void)
{
	//Ê¹ÄÜÊ±ÖÓGPIOB£¬CMP
    rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_CFGCMP);
	
	//ÉèÖÃÒý½ÅÄ£Ê½£¬ÉÏÀ­
	gpio_mode_set(GPIOB,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_PIN_4);
	
	//ÖÐ¶ÏÏßÊ¹ÄÜ
	nvic_irq_enable(EXTI4_15_IRQn,3U);
	
	//ÅäÖÃÖÐ¶ÏÏß
	syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN4);
	
	//³õÊ¼»¯ÖÐ¶ÏÏß£¬ÉèÖÃÎªÖÐ¶ÏÄ£Ê½£¬ÉÏÉýÑØÏÂ½µÑØ´¥·¢
	exti_init(EXTI_4,EXTI_INTERRUPT,EXTI_TRIG_BOTH);
	
	//ÖÐ¶Ï±êÖ¾Î»Çå³ý
	exti_interrupt_flag_clear(EXTI_4);
}

extern MENU_OptionTypeDef MENU_OptionList[];

/*
*   º¯ÊýÄÚÈÝ£º°´¼ü´¦Àíº¯Êý
*   º¯Êý²ÎÊý£ºÎÞ
*   ·µ»ØÖµ£º  ÎÞ
*/
void Key_Handle(volatile struct Oscilloscope *value)
{
	uint8_t i=0,j=0;
	float tempValue=0;
	switch((*value).keyValue)
	{
		case KEY1PRESS:
            (*value).pwmOut=((uint32_t)(*value).timerPeriod*0.04f)+(*value).pwmOut;
            if((*value).pwmOut > (*value).timerPeriod)
            {
                (*value).pwmOut = 0;
            }
            Set_Output_PWMComparex((*value).pwmOut);
			break;
        case KEY3PRESS:
            tempValue=(*value).pwmOut/((*value).timerPeriod+0.0f);
            (*value).timerPeriod = (*value).timerPeriod/2;
            if((*value).timerPeriod < 250)
            {
                (*value).timerPeriod = 1000;
            }
            (*value).outputFreq = 1000000/(*value).timerPeriod;
            (*value).pwmOut = (uint16_t)((*value).timerPeriod*tempValue);
            Set_Output_PWMComparex((*value).pwmOut);
            Set_Output_Freq((*value).timerPeriod-1);
            tempValue=0;
						break;
        case KEY2PRESS:
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
            break;
				 case KEYDPRESS:
					  if((*value).isSel!=0){
							MENU_OptionList[(*value).menuSel].funchand(value,KEYDPRESS);
						}else{
							MENU_SEL_HANDLER(value,KEYDPRESS);
						}
						break;
        case KEYAPRESS:
						if((*value).isSel!=0){
							MENU_OptionList[(*value).menuSel].funchand(value,KEYAPRESS);
						}else{
							MENU_SEL_HANDLER(value,KEYAPRESS);
						}
						break;
        case KEYBPRESS:
						if((*value).isSel!=0){
							MENU_OptionList[(*value).menuSel].funchand(value,KEYBPRESS);
						}else{
							MENU_SEL_HANDLER(value,KEYBPRESS);
						}
						break;
		default:
			break;
	}
    (*value).keyValue=0;
 
}



void Key_Sacnf(volatile struct Oscilloscope *value)
{
	if(key1_state == KEYPRESS){
		delay_1ms(20);
		if(gpio_input_bit_get(GPIOB,GPIO_PIN_13) == RESET){
			(*value).keyValue = KEY1PRESS;
			key1_state = NoPRESS;
		}
	}
	else{
		key1_state = NoPRESS;
	}
	
	if(key2_state == KEYPRESS){
		delay_1ms(20);
		if(gpio_input_bit_get(GPIOB,GPIO_PIN_14) == RESET){
			(*value).keyValue = KEY2PRESS;
			key2_state = NoPRESS;
		}
	}
	else{
		key2_state = NoPRESS;
	}
	
	if(key3_state == KEYPRESS){
		delay_1ms(20);
		if(gpio_input_bit_get(GPIOB,GPIO_PIN_15) == RESET){
			(*value).keyValue = KEY3PRESS;
			key3_state = NoPRESS;
		}
	}
	else{
		key3_state = NoPRESS;
	}
	
	if(key4_state == KEYPRESS){
		delay_1ms(20);
		if(gpio_input_bit_get(GPIOB,GPIO_PIN_9) == RESET){
			(*value).keyValue = KEYDPRESS;
			key4_state = NoPRESS;
		}
	}
	else{
		key4_state = NoPRESS;
	}
}



extern volatile struct Oscilloscope oscilloscope;


void EXTI4_15_IRQHandler(void)
{
    static uint8_t A_cnt=0;
    static uint8_t B_value=0;
    if(RESET != exti_interrupt_flag_get(EXTI_4)) //for rotary encoder
    {
				if((gpio_input_bit_get(GPIOB,GPIO_PIN_4) == RESET) && (A_cnt == 0))	//AÏàÏÂ½µÑØ´¥·¢Ò»´Î
				{
					A_cnt++;			//¼ÆÊýÖµ¼ÓÒ»£¬±íÊ¾ÒÑ¾­´¥·¢ÁËµÚÒ»´ÎÖÐ¶Ï
					B_value = 0;	//¶ÁÈ¡BÏàµçÆ½£¬ÈôÎª¸ßµçÆ½ÔòB_levelÖÃ1£¬·´Ö®±£³Ö0
					if(gpio_input_bit_get(GPIOB,GPIO_PIN_3) == SET)
					{
						B_value = 1;
					}
				}
				else if((gpio_input_bit_get(GPIOB,GPIO_PIN_4) == SET) && (A_cnt == 1))	//AÏàÉÏÉýÑØ´¥·¢Ò»´Î
				{
					A_cnt = 0;
					if((B_value == 1) && (gpio_input_bit_get(GPIOB,GPIO_PIN_3) == RESET))
					{
						oscilloscope.keyValue=KEYBPRESS; 
					}
					if((B_value == 0) && (gpio_input_bit_get(GPIOB,GPIO_PIN_3) == SET))
					{
						oscilloscope.keyValue=KEYAPRESS;
					}
				}
        exti_interrupt_flag_clear(EXTI_4);
    }
    if(RESET != exti_interrupt_flag_get(EXTI_13))
    {
       key1_state = KEYPRESS;
       exti_interrupt_flag_clear(EXTI_13);
    }
    if(RESET != exti_interrupt_flag_get(EXTI_14))
    {
       key2_state = KEYPRESS;
       exti_interrupt_flag_clear(EXTI_14);
    }
    if(RESET != exti_interrupt_flag_get(EXTI_15))
    {
       key3_state = KEYPRESS;
       exti_interrupt_flag_clear(EXTI_15);
    }
		if(RESET != exti_interrupt_flag_get(EXTI_9))
    {
       key4_state = KEYPRESS;
       exti_interrupt_flag_clear(EXTI_9);
    }
}
