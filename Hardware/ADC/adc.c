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
    
    //ADC连续功能使能//TODO
    adc_special_function_config(ADC_CONTINUOUS_MODE, DISABLE); 

    adc_discontinuous_mode_config(ADC_REGULAR_CHANNEL, 1);

    //ADC扫描功能失能，这里仅一个通道
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);

    //ADC注入组自动转换模式失能，这里无需注入组
    adc_special_function_config(ADC_INSERTED_CHANNEL_AUTO, DISABLE);    
    
    //ADC数据校准配置 
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    
    //ADC通道长度配置
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);   

    //ADC常规通道配置--PA3，顺序组0，通道3，采样时间55.5个时钟周期
    adc_regular_channel_config(0, ADC_CHANNEL_3, ADC_SAMPLETIME_55POINT5);    
    
    //ADC触发器配置，软件触发
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_T0_CH0);
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
    
    //使能ADC
    adc_enable();
    delay_ms(1U);
    
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

void ADC_TRIG_TIMMER_Init(){
//	//定时器通道输入参数结构体
//    timer_oc_parameter_struct timer_ocinitpara;
//    //定时器参数结构体
//    timer_parameter_struct timer_initpara;
//    rcu_periph_clock_enable(RCU_TIMER0);

//    //复位定时器
//    timer_deinit(TIMER0);
//    
//    //定时器参数初始化
//    timer_struct_para_init(&timer_initpara);
//    
//    timer_initpara.prescaler         = 71;                  //预分频器参数1M
//    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;  //边沿对齐
//    timer_initpara.counterdirection  = TIMER_COUNTER_UP;    //向上计数
//    timer_initpara.period            = 500;               //周期
//    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;    //时钟分频
//		timer_initpara.repetitioncounter = 0;								    //重装载值
//    timer_init(TIMER0, &timer_initpara);                    //参数初始化
//    
//    //定时器通道输入参数初始化
//    timer_channel_output_struct_para_init(&timer_ocinitpara);
//    
//		timer_ocinitpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
//    timer_ocinitpara.outputstate = TIMER_CCX_ENABLE;
//    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);

//	  //使能自动重装载
//	  timer_auto_reload_shadow_enable(TIMER0);
//	
//		timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 100);
//    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM1);
//    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
//		timer_primary_output_config(TIMER0, ENABLE);//important
//    timer_enable(TIMER0);





//定时器输出参数结构体
	timer_oc_parameter_struct timer_ocinitpara;
	
	//定时器初始化参数结构体
   timer_parameter_struct timer_initpara;
	

	//使能定时器0
	rcu_periph_clock_enable(RCU_TIMER0);
	
	//使能时钟
	rcu_periph_clock_enable(RCU_GPIOA);
	
	//GPIO复用模式设置--PA2-TIMER14_CH0
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
	
	//输出类型设置
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_4);
	
	//复用模式0
	gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_4);
	
	//复位定时器14
	timer_deinit(TIMER0);
	
	//初始化定时器结构体参数
	timer_struct_para_init(&timer_initpara);
	
	timer_initpara.prescaler         = 71;									//预分频器参数
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;	                //边沿对齐
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;		            //向上计数
	timer_initpara.period            = 9;								//周期
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;		            //时钟分频
	timer_initpara.repetitioncounter = 0;								    //重装载值
	timer_init(TIMER0, &timer_initpara);
	
	//初始化定时器通道输出参数结构体
	timer_channel_output_struct_para_init(&timer_ocinitpara);
	
	timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;				//输出状态，主输出通道开启
	timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;			    //互补输出状态关闭
	timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_LOW;	        //输出极性为高
	timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;        //互补输出极性为高
	timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_HIGH;        //空闲状态通道输出
	timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_HIGH;       //空闲状态互补通道输出
	
	timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);
	
	//输出比较值
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 2);
	
	//输出模式0，当计时器小于比较值时，输出有效电平，为高，大于比较器值时输出为低
	timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM1);
	
	//影子模式输出关闭
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
	
	//使能自动重装载
	timer_auto_reload_shadow_enable(TIMER0);

	
	//配置定时器为主要输出函数，所有通道使能
	timer_primary_output_config(TIMER0, ENABLE);

	timer_enable(TIMER0);	


}