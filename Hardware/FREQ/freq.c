#include "freq.h"
#include "main.h"

void Init_FreqTimer(void)
{
    //定时器通道输入参数结构体
    timer_ic_parameter_struct timer_icinitpara;
    
    //定时器参数结构体
    timer_parameter_struct timer_initpara;
    
    //使能时钟
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_TIMER2);
    
    //设置引脚模式
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    
    //设置输出状态
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    
    //设置为复用功能
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_6);
    
    //定时器中断使能
    nvic_irq_enable(TIMER2_IRQn, 2U);
    
    //复位定时器
    timer_deinit(TIMER2);
    
    //定时器参数初始化
    timer_struct_para_init(&timer_initpara);
    
    timer_initpara.prescaler         = 71;                  //预分频器参数
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;  //边沿对齐
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;    //向上计数
    timer_initpara.period            = 65535;               //周期
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;    //时钟分频
    timer_init(TIMER2, &timer_initpara);                    //参数初始化
    
    //定时器通道输入参数初始化
    timer_channel_input_struct_para_init(&timer_icinitpara);
    
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;        //通道输入极性
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;     //通道输入模式选择
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;               //通道输入捕获预分频
    timer_icinitpara.icfilter    = 0x00;                             //通道输入捕获滤波
    timer_input_capture_config(TIMER2,TIMER_CH_0,&timer_icinitpara);
    
    //使能自动重装载值
    timer_auto_reload_shadow_enable(TIMER2);
    
    //清除中断标志位
    timer_interrupt_flag_clear(TIMER2,TIMER_INT_FLAG_CH0);
    
    //使能定时器通道中断
    timer_interrupt_enable(TIMER2,TIMER_INT_CH0);
    
    //定时器中断使能
    timer_enable(TIMER2);
}

static __IO uint16_t ccnumber = 0;
static __IO uint32_t freq = 0;
static __IO uint16_t readvalue1 = 0, readvalue2 = 0;
static __IO uint32_t count = 0;

extern volatile struct Oscilloscope oscilloscope;

#define WINDOW_SIZE 20
#define TIMER_FREQUENCY 1000000U //(1 MHz)
volatile uint32_t captureValues[WINDOW_SIZE];
volatile uint32_t captureIndex = 0;
volatile uint32_t captureCount = 0;
void TIMER2_IRQHandler(void)
{
  if(SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_CH0))
  {
      
      if(0 == ccnumber){
            // 读第一次通道0捕获值
            readvalue1 = timer_channel_capture_value_register_read(TIMER2, TIMER_CH_0);
            ccnumber = 1;
        }else if(1 == ccnumber){
            // 读第2次通道0捕获值 
            readvalue2 = timer_channel_capture_value_register_read(TIMER2, TIMER_CH_0);
            // 如果第二次捕获值大于第一次
            if(readvalue2 > readvalue1){
                count = (readvalue2 - readvalue1); 
            }else{
                count = ((0xFFFFU - readvalue1) + readvalue2); 
            }				
					  captureValues[captureIndex] = count;
            captureIndex = (captureIndex + 1) % WINDOW_SIZE;

            if (captureCount < WINDOW_SIZE) {
                captureCount++;
            }
            if (captureCount == WINDOW_SIZE) {
                uint32_t totalDiff = 0;
                for (int i = 0; i < WINDOW_SIZE; i++) {
                    totalDiff += captureValues[i];
                }
                uint32_t avgDiff = totalDiff / WINDOW_SIZE;
                freq = TIMER_FREQUENCY / avgDiff;

                oscilloscope.gatherFreq = freq;
								captureCount=0;
            }
									
            ccnumber = 0;
        }
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH0);
  }
}