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
*   函数内容：得到ADC值
*   函数参数：value--数组下标
*   返回值：  无
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
*   函数内容：初始化ADC
*   函数参数：无
*   返回值：    无
*/
void Init_ADC(void)
{
    //使能引脚
    rcu_periph_clock_enable(RCU_GPIOA);
    
    //使能ADC时钟
    rcu_periph_clock_enable(RCU_ADC);
    
    //使能时钟配置，最大28M
    rcu_adc_clock_config(RCU_ADCCK_AHB_DIV9);
    
    //引脚配置，PA3，模拟输入，无上下拉
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_3);
    
    //ADC连续功能使能
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE); 

    //ADC扫描功能失能，这里仅一个通道
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);

    //ADC注入组自动转换模式失能，这里无需注入组
    adc_special_function_config(ADC_INSERTED_CHANNEL_AUTO, DISABLE);    
    
    //ADC数据校准配置
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    
    //ADC通道长度配置
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);   

    //ADC常规通道配置--PA3，顺序组0，通道3，采样时间55.5个时钟周期
    adc_regular_channel_config(0, ADC_CHANNEL_3, ADC_SAMPLETIME_239POINT5);    
    
    //ADC触发器配置，软件触发
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE); 
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
    
    //使能ADC
    adc_enable();
    delay_1ms(1U);
    
    //使能校准和复位
    adc_calibration_enable();
    
    //DMA模式使能
    adc_dma_mode_enable();
    
    //ADC软件触发使能
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}

void ADC_DMA_Init(void)
{
    //DMA时钟使能
    rcu_periph_clock_enable(RCU_DMA);
    
    //DMA参数结构体
    dma_parameter_struct dma_data_parameter;
    
    //使能DMA中断
    nvic_irq_enable(DMA_Channel0_IRQn, 0U);
    
    //通道0复位
    dma_deinit(DMA_CH0);
    
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA);       //外设基地址
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;  //外设地址不自增
    dma_data_parameter.memory_addr  = (uint32_t)(&adc_value);       //内存地址
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;   //内存地址自增
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;   //外设位宽
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;       //内存位宽
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;     //外设到内存
    dma_data_parameter.number       = ADC_VALUE_NUM;                //数量
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;            //高优先级
    
    dma_init(DMA_CH0, &dma_data_parameter);                         //DMA通道0初始化
    
    dma_circulation_enable(DMA_CH0);                               //DMA循环模式使能
    
    dma_channel_enable(DMA_CH0);                                    //DMA通道0使能
    
    //使能DMA传输完成中断
    dma_interrupt_enable(DMA_CH0, DMA_CHXCTL_FTFIE);
}

extern volatile struct Oscilloscope oscilloscope;

void DMA_Channel0_IRQHandler(void)
{
    if(dma_interrupt_flag_get(DMA_CH0, DMA_INT_FLAG_FTF)){
        oscilloscope.showbit=1;
        dma_channel_disable(DMA_CH0);
        //清除中断标志位
        dma_interrupt_flag_clear(DMA_CH0, DMA_INT_FLAG_G);
    }
}