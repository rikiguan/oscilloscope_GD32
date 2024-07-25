#include "freq.h"
#include "main.h"

void Init_FreqTimer(void)
{
    //��ʱ��ͨ����������ṹ��
    timer_ic_parameter_struct timer_icinitpara;
    
    //��ʱ�������ṹ��
    timer_parameter_struct timer_initpara;
    
    //ʹ��ʱ��
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_TIMER2);
    
    //��������ģʽ
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    
    //�������״̬
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    
    //����Ϊ���ù���
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_6);
    
    //��ʱ���ж�ʹ��
    nvic_irq_enable(TIMER2_IRQn, 2U);
    
    //��λ��ʱ��
    timer_deinit(TIMER2);
    
    //��ʱ��������ʼ��
    timer_struct_para_init(&timer_initpara);
    
    timer_initpara.prescaler         = 71;                  //Ԥ��Ƶ������
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;  //���ض���
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;    //���ϼ���
    timer_initpara.period            = 65535;               //����
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;    //ʱ�ӷ�Ƶ
    timer_init(TIMER2, &timer_initpara);                    //������ʼ��
    
    //��ʱ��ͨ�����������ʼ��
    timer_channel_input_struct_para_init(&timer_icinitpara);
    
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;        //ͨ�����뼫��
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;     //ͨ������ģʽѡ��
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;               //ͨ�����벶��Ԥ��Ƶ
    timer_icinitpara.icfilter    = 0x00;                             //ͨ�����벶���˲�
    timer_input_capture_config(TIMER2,TIMER_CH_0,&timer_icinitpara);
    
    //ʹ���Զ���װ��ֵ
    timer_auto_reload_shadow_enable(TIMER2);
    
    //����жϱ�־λ
    timer_interrupt_flag_clear(TIMER2,TIMER_INT_FLAG_CH0);
    
    //ʹ�ܶ�ʱ��ͨ���ж�
    timer_interrupt_enable(TIMER2,TIMER_INT_CH0);
    
    //��ʱ���ж�ʹ��
    timer_enable(TIMER2);
}

static __IO uint16_t ccnumber = 0;
static __IO uint32_t freq = 0;
static __IO uint16_t readvalue1 = 0, readvalue2 = 0;
static __IO uint32_t count = 0;

extern volatile struct Oscilloscope oscilloscope;

void Freq_calibration(__IO uint32_t *freq)
{
	if(((*freq) >= 950) && ((*freq) < 1050)){
		(*freq) = 1000;
	}
	else if(((*freq) >= 1050) && ((*freq) < 2050)){
		(*freq) = 2000;
	}
	else if(((*freq) >= 2050) && ((*freq) < 3050)){
		(*freq) = 3000;
	}
	else if(((*freq) >= 3050) && ((*freq) < 4050)){
		(*freq) = 4000;
	}
	else if(((*freq) >= 4050) && ((*freq) < 5050)){
		(*freq) = 5000;
	}
	else if(((*freq) >= 5050) && ((*freq) < 6050)){
		(*freq) = 6000;
	}
	else if(((*freq) >= 6050) && ((*freq) < 7050)){
		(*freq) = 7000;
	}
	else if(((*freq) >= 7050) && ((*freq) < 8050)){
		(*freq) = 8000;
	}
	else if(((*freq) >= 8050) && ((*freq) < 9050)){
		(*freq) = 9000;
	}
	else if(((*freq) >= 9050) && ((*freq) < 10050)){
		(*freq) = 10000;
	}
}

void TIMER2_IRQHandler(void)
{
  if(SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_CH0))
  {
      
      if(0 == ccnumber){
            // ����һ��ͨ��0����ֵ
            readvalue1 = timer_channel_capture_value_register_read(TIMER2, TIMER_CH_0);
            ccnumber = 1;
        }else if(1 == ccnumber){
            // ����2��ͨ��0����ֵ 
            readvalue2 = timer_channel_capture_value_register_read(TIMER2, TIMER_CH_0);
            // ����ڶ��β���ֵ���ڵ�һ��
            if(readvalue2 > readvalue1){
                count = (readvalue2 - readvalue1); 
            }else{
                count = ((0xFFFFU - readvalue1) + readvalue2); 
            }
            
            //����Ƶ��
            freq = 1000000U / count;     
            //Freq_calibration(&freq);
            oscilloscope.gatherFreq = freq; 
         
            readvalue1=0;readvalue2=0;
            count=0;
            freq=0;
            ccnumber = 0;
        }
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH0);
  }
}