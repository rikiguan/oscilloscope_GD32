#include "led.h"

/*
*   函数内容：初始化LED引脚
*   函数参数：无
*   返回值：  无
*/
void Init_LED_GPIO(void)
{
    //使能时钟
    rcu_periph_clock_enable(RCU_GPIOC);
    //设置输出模式，不上下拉
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_14);
    //设置输出模式，不上下拉
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    //设置输出类型，推挽输出，50Mhz
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    //设置输出类型，推挽输出，50Mhz
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    //默认不开灯
    CLose_LED(led2);
    CLose_LED(led3);
}

/*
*   函数内容：打开对应LED灯
*   函数参数：无
*   返回值：  无
*/
void Open_LED(uint8_t value)
{
    switch(value)
    {
        case 1:
            gpio_bit_reset(GPIOC,GPIO_PIN_14);
            break;
        case 2:
            gpio_bit_reset(GPIOC,GPIO_PIN_15);
            break;
        default:
            break;
    }
}

/*
*   函数内容：关闭对应LED灯
*   函数参数：无
*   返回值：  无
*/
void CLose_LED(uint8_t value)
{
    switch(value)
    {
        case 1:
            gpio_bit_set(GPIOC,GPIO_PIN_14);
            break;
        case 2:
            gpio_bit_set(GPIOC,GPIO_PIN_15);
            break;
        default:
            break;
    }
}