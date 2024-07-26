#include "adc.h"
#include "main.h"

#include "update.h"
#define ADC_VALUE_NUM 300U

uint16_t adc_value[ADC_VALUE_NUM];
uint16_t filteredValues[ADC_VALUE_NUM];
#define WindowSize 5

void Opt_ADC_Value()
{
    medianFilter(adc_value,filteredValues,ADC_VALUE_NUM,WindowSize);
}


/*
*   �������ݣ��õ�ADCֵ
*   ����������value--�����±�
*   ����ֵ��  ��
*/
uint16_t Get_ADC_Value(uint16_t value)
{
    uint16_t returnValue=0;
    if(value>ADC_VALUE_NUM)
    {
        value=0;
    }
    returnValue=filteredValues[value];
    filteredValues[value]=0;
    return returnValue;
}
/*
*   �������ݣ���ʼ��ADC
*   ������������
*   ����ֵ��    ��
*/
void Init_ADC(void)
{
    //ʹ������
    rcu_periph_clock_enable(RCU_GPIOA);
    
    //ʹ��ADCʱ��
    rcu_periph_clock_enable(RCU_ADC);
    
    //ʹ��ʱ�����ã����28M
    rcu_adc_clock_config(RCU_ADCCK_AHB_DIV9);
    
    //�������ã�PA3��ģ�����룬��������
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_3);
    
    //ADC��������ʹ��
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE); 

    //ADCɨ�蹦��ʧ�ܣ������һ��ͨ��
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);

    //ADCע�����Զ�ת��ģʽʧ�ܣ���������ע����
    adc_special_function_config(ADC_INSERTED_CHANNEL_AUTO, DISABLE);    
    
    //ADC����У׼����
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    
    //ADCͨ����������
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);   

    //ADC����ͨ������--PA3��˳����0��ͨ��3������ʱ��55.5��ʱ������
    adc_regular_channel_config(0, ADC_CHANNEL_3, ADC_SAMPLETIME_239POINT5);    
    
    //ADC���������ã��������
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE); 
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
    
    //ʹ��ADC
    adc_enable();
    delay_1ms(1U);
    
    //ʹ��У׼�͸�λ
    adc_calibration_enable();
    
    //DMAģʽʹ��
    adc_dma_mode_enable();
    
    //ADC�������ʹ��
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}

void ADC_DMA_Init(void)
{
    //DMAʱ��ʹ��
    rcu_periph_clock_enable(RCU_DMA);
    
    //DMA�����ṹ��
    dma_parameter_struct dma_data_parameter;
    
    //ʹ��DMA�ж�
    nvic_irq_enable(DMA_Channel0_IRQn, 0U);
    
    //ͨ��0��λ
    dma_deinit(DMA_CH0);
    
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA);       //�������ַ
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;  //�����ַ������
    dma_data_parameter.memory_addr  = (uint32_t)(&adc_value);       //�ڴ��ַ
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;   //�ڴ��ַ����
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;   //����λ��
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;       //�ڴ�λ��
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;     //���赽�ڴ�
    dma_data_parameter.number       = ADC_VALUE_NUM;                //����
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;            //�����ȼ�
    
    dma_init(DMA_CH0, &dma_data_parameter);                         //DMAͨ��0��ʼ��
    
    dma_circulation_enable(DMA_CH0);                               //DMAѭ��ģʽʹ��
    
    dma_channel_enable(DMA_CH0);                                    //DMAͨ��0ʹ��
    
    //ʹ��DMA��������ж�
    dma_interrupt_enable(DMA_CH0, DMA_CHXCTL_FTFIE);
}

extern volatile struct Oscilloscope oscilloscope;

void DMA_Channel0_IRQHandler(void)
{
    if(dma_interrupt_flag_get(DMA_CH0, DMA_INT_FLAG_FTF)){
        oscilloscope.showbit=1;
        dma_channel_disable(DMA_CH0);
        //����жϱ�־λ
        dma_interrupt_flag_clear(DMA_CH0, DMA_INT_FLAG_G);
    }
}

void ADC_TRIG_TIMMWE_Init(){
	 //��ʱ��ͨ����������ṹ��
    timer_ic_parameter_struct timer_icinitpara;
    
    //��ʱ�������ṹ��
    timer_parameter_struct timer_initpara;
    

    rcu_periph_clock_enable(RCU_TIMER0);
    

    //��ʱ���ж�ʹ��
    nvic_irq_enable(TIMER2_IRQn, 2U);
    
    //��λ��ʱ��
    timer_deinit(TIMER2);
    
    //��ʱ��������ʼ��
    timer_struct_para_init(&timer_initpara);
    
    timer_initpara.prescaler         = 71;                  //Ԥ��Ƶ������1M
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