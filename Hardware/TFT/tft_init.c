#include "tft_init.h"


//#define TFT_SCL PA5
//#define TFT_SDA PA7
//#define TFT_RES PB5
//#define TFT_CS  PB7
//#define TFT_BLK PB8

/*
*   函数内容：初始化SPI0引脚
*   函数参数：无
*   返回值：无
*/
static void Init_SPI0_GPIO(void)
{
    //spi结构体参数
    spi_parameter_struct spi_init_struct;
    
    //使能GPIOA时钟
    rcu_periph_clock_enable(RCU_GPIOA);
    
    //使能SPI0时钟
    rcu_periph_clock_enable(RCU_SPI0);
    
    //设置复用功能
    gpio_af_set(GPIOA, GPIO_AF_0,   GPIO_PIN_5 | GPIO_PIN_7);
    
    //模式设置
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5 | GPIO_PIN_7);
    
    //输出设置
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    
    //SPI0复位
    spi_i2s_deinit(SPI0);
    
    //SPI参数初始化
    spi_struct_para_init(&spi_init_struct);
    
    spi_init_struct.trans_mode           = SPI_TRANSMODE_BDTRANSMIT;//单线双向半双工发送模式
    spi_init_struct.device_mode          = SPI_MASTER;              //主机模式
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;      //8位数据位宽
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE; //极性高，第2个边缘采样
    spi_init_struct.nss                  = SPI_NSS_SOFT;            //NSS软件控制
    spi_init_struct.prescale             = SPI_PSC_2;               //2分频
    spi_init_struct.endian               = SPI_ENDIAN_MSB;          //高位在前
    spi_init(SPI0, &spi_init_struct);
    
    spi_enable(SPI0);
}

/*
*   函数内容：初始化SPI0引脚-16位
*   函数参数：无
*   返回值：无
*/
void Init_SPI0_GPIO16(void)
{
    //spi结构体参数
    spi_parameter_struct spi_init_struct;
    
    //使能GPIOA时钟
    rcu_periph_clock_enable(RCU_GPIOA);
    
    //使能SPI0时钟
    rcu_periph_clock_enable(RCU_SPI0);
    
    //设置复用功能
    gpio_af_set(GPIOA, GPIO_AF_0,   GPIO_PIN_5 | GPIO_PIN_7);
    
    //模式设置
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5 | GPIO_PIN_7);
    
    //输出设置
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    
    //SPI0复位
    spi_i2s_deinit(SPI0);
    
    //SPI参数初始化
    spi_struct_para_init(&spi_init_struct);
    
    spi_init_struct.trans_mode           = SPI_TRANSMODE_BDTRANSMIT;//双线单向全双工模式
    spi_init_struct.device_mode          = SPI_MASTER;              //主机模式
    spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;     //8位数据位宽
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE; //极性高，第2个边缘采样
    spi_init_struct.nss                  = SPI_NSS_SOFT;            //NSS软件控制
    spi_init_struct.prescale             = SPI_PSC_2;               //2分频
    spi_init_struct.endian               = SPI_ENDIAN_MSB;          //高位在前
    spi_init(SPI0, &spi_init_struct);
    
    //使能SPI0
    spi_enable(SPI0);
}

/*
*   函数内容：SPI0发送数据
*   函数参数：无
*   返回值：无
*/
static void SPI0_Write(uint8_t data)
{
    //等待发送缓冲区空闲
    while(spi_i2s_flag_get(SPI0,SPI_FLAG_TBE)==RESET);
    
    //发送数据
    spi_i2s_data_transmit(SPI0, data);  
}

/*
*   函数内容：SPI0发送数据--16位
*   函数参数：无
*   返回值：无
*/
static void SPI0_Write16(uint16_t data)
{
    //等待发送缓冲区空闲
    while(spi_i2s_flag_get(SPI0,SPI_FLAG_TBE)==RESET);
    
    //发送数据
    spi_i2s_data_transmit(SPI0, data);   
}

/*
*   函数内容：初始化TFT其余引脚
*   函数参数：无
*   返回值：无
*/
static void TFT_GPIO_Init(void)
{
    //使能时钟
    rcu_periph_clock_enable(RCU_GPIOB);
    
    //设置输出模式，不上下拉
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8);
    
    //设置输出类型，推挽输出，50Mhz
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8);

    gpio_bit_set(GPIOB,GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8);
}

/*
*   函数内容：TFT发送单个字节数据
*   函数参数：无
*   返回值：无
*/
void TFT_WR_DATA8(uint8_t data)
{
    gpio_bit_reset(GPIOB,GPIO_PIN_7);   //拉低片选信号
    
    SPI0_Write(data);
    
    gpio_bit_set(GPIOB,GPIO_PIN_7);     //拉高片选信号
}

/*
*   函数内容：TFT发送2个字节数据
*   函数参数：无
*   返回值：无
*/
void TFT_WR_DATA(uint16_t data)
{
    gpio_bit_reset(GPIOB,GPIO_PIN_7);   //拉低片选信号
    
    SPI0_Write(data>>8);
    
    SPI0_Write(data);
    
    gpio_bit_set(GPIOB,GPIO_PIN_7);     //拉高片选信号
}

/*
*   函数内容：TFT发送2个字节数据，每次发送16个字节
*   函数参数：无
*   返回值：无
*/
void TFT_WR_DATA16(uint16_t data)
{
    gpio_bit_reset(GPIOB,GPIO_PIN_7);   //拉低片选信号
    
    SPI0_Write16(data);
    
    gpio_bit_set(GPIOB,GPIO_PIN_7);     //拉高片选信号
}

/*
*   函数内容：TFT发送命令数据
*   函数参数：无
*   返回值：无
*/
void TFT_WR_REG(uint8_t reg)
{
    gpio_bit_reset(GPIOB,GPIO_PIN_6);   //拉低命令信号
    gpio_bit_reset(GPIOB,GPIO_PIN_7);   //拉低片选信号
    
    SPI0_Write(reg);
    
    gpio_bit_set(GPIOB,GPIO_PIN_6);     //拉高命令信号
    gpio_bit_set(GPIOB,GPIO_PIN_7);     //拉高片选信号
}

/*
*   函数内容：TFT发送命令数据，单次发送是16个字节
*   函数参数：无
*   返回值：无
*/
void TFT_WR_REG16(uint16_t reg)
{
    gpio_bit_reset(GPIOB,GPIO_PIN_6);   //拉低命令信号
    gpio_bit_reset(GPIOB,GPIO_PIN_7);   //拉低片选信号
    
    SPI0_Write16(reg);
    
    gpio_bit_set(GPIOB,GPIO_PIN_6);     //拉高命令信号
    gpio_bit_set(GPIOB,GPIO_PIN_7);     //拉高片选信号
}

void TFT_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
//	if(USE_HORIZONTAL==0)
//	{
//		TFT_WR_REG(0x2a);//列地址设置
//		TFT_WR_DATA(x1);
//		TFT_WR_DATA(x2);
//		TFT_WR_REG(0x2b);//行地址设置
//		TFT_WR_DATA(y1);
//		TFT_WR_DATA(y2);
//		TFT_WR_REG(0x2c);//储存器写
//	}
//	else if(USE_HORIZONTAL==1)
//	{
//		TFT_WR_REG(0x2a);//列地址设置
//		TFT_WR_DATA(x1);
//		TFT_WR_DATA(x2);
//		TFT_WR_REG(0x2b);//行地址设置
//		TFT_WR_DATA(y1);
//		TFT_WR_DATA(y2);
//		TFT_WR_REG(0x2c);//储存器写
//	}
//	else if(USE_HORIZONTAL==2)
//	{
//		TFT_WR_REG(0x2a);//列地址设置
//		TFT_WR_DATA(x1);
//		TFT_WR_DATA(x2);
//		TFT_WR_REG(0x2b);//行地址设置
//		TFT_WR_DATA(y1);
//		TFT_WR_DATA(y2);
//		TFT_WR_REG(0x2c);//储存器写
//	}
//	else
//	{
//		TFT_WR_REG(0x2a);//列地址设置
//		TFT_WR_DATA(x1);
//		TFT_WR_DATA(x2);
//		TFT_WR_REG(0x2b);//行地址设置
//		TFT_WR_DATA(y1);
//		TFT_WR_DATA(y2);
//		TFT_WR_REG(0x2c);//储存器写
//	}    
		TFT_WR_REG(0x2a);//列地址设置
		TFT_WR_DATA(x1+1);
		TFT_WR_DATA(x2+1);
		TFT_WR_REG(0x2b);//行地址设置
		TFT_WR_DATA(y1+2);
		TFT_WR_DATA(y2+2);
		TFT_WR_REG(0x2c);//储存器写
}

/*
*   函数内容：设置显示地址，每次发送16位数据
*   函数参数：无
*   返回值：无
*/
void TFT_Address_Set16(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
//	if(USE_HORIZONTAL==0)
//	{
//		TFT_WR_REG16(0x2a);//列地址设置
//		TFT_WR_DATA16(x1);
//		TFT_WR_DATA16(x2);
//		TFT_WR_REG16(0x2b);//行地址设置
//		TFT_WR_DATA16(y1);
//		TFT_WR_DATA16(y2);
//		TFT_WR_REG16(0x2c);//储存器写
//	}
//	else if(USE_HORIZONTAL==1)
//	{
//		TFT_WR_REG16(0x2a);//列地址设置
//		TFT_WR_DATA16(x1);
//		TFT_WR_DATA16(x2);
//		TFT_WR_REG16(0x2b);//行地址设置
//		TFT_WR_DATA16(y1);
//		TFT_WR_DATA16(y2);
//		TFT_WR_REG16(0x2c);//储存器写
//	}
//	else if(USE_HORIZONTAL==2)
//	{
//		TFT_WR_REG16(0x2a);//列地址设置
//		TFT_WR_DATA16(x1);
//		TFT_WR_DATA16(x2);
//		TFT_WR_REG16(0x2b);//行地址设置
//		TFT_WR_DATA16(y1);
//		TFT_WR_DATA16(y2);
//		TFT_WR_REG16(0x2c);//储存器写
//	}
//	else
//	{
//		TFT_WR_REG16(0x2a);//列地址设置
//		TFT_WR_DATA16(x1);
//		TFT_WR_DATA16(x2);
//		TFT_WR_REG16(0x2b);//行地址设置
//		TFT_WR_DATA16(y1);
//		TFT_WR_DATA16(y2);
//		TFT_WR_REG16(0x2c);//储存器写
//	}    
		TFT_WR_REG(0x2a);//列地址设置
		TFT_WR_DATA16(x1+1);
		TFT_WR_DATA16(x2+1);
		TFT_WR_REG(0x2b);//行地址设置
		TFT_WR_DATA16(y1+2);
		TFT_WR_DATA16(y2+2);
		TFT_WR_REG(0x2c);//储存器写
}

void TFT_Init(void)
{
    TFT_GPIO_Init();
    Init_SPI0_GPIO();
    
    gpio_bit_reset(GPIOB,GPIO_PIN_5);   //复位
    delay_ms(100);
    gpio_bit_set(GPIOB,GPIO_PIN_5);     //
    delay_ms(100);
    
    gpio_bit_set(GPIOB,GPIO_PIN_8);     //打开背光
    delay_ms(100);
    
	//************* Start Initial Sequence **********//
	TFT_WR_REG(0x11); //Sleep out 
	delay_ms(120);              //Delay 120ms 
	//------------------------------------ST7735S Frame Rate-----------------------------------------// 
	TFT_WR_REG(0xB1); 
	TFT_WR_DATA8(0x05); 
	TFT_WR_DATA8(0x3C); 
	TFT_WR_DATA8(0x3C); 
	TFT_WR_REG(0xB2); 
	TFT_WR_DATA8(0x05);
	TFT_WR_DATA8(0x3C); 
	TFT_WR_DATA8(0x3C); 
	TFT_WR_REG(0xB3); 
	TFT_WR_DATA8(0x05); 
	TFT_WR_DATA8(0x3C); 
	TFT_WR_DATA8(0x3C); 
	TFT_WR_DATA8(0x05); 
	TFT_WR_DATA8(0x3C); 
	TFT_WR_DATA8(0x3C); 
	//------------------------------------End ST7735S Frame Rate---------------------------------// 
	TFT_WR_REG(0xB4); //Dot inversion 
	TFT_WR_DATA8(0x03); 
	//------------------------------------ST7735S Power Sequence---------------------------------// 
	TFT_WR_REG(0xC0); 
	TFT_WR_DATA8(0x28); 
	TFT_WR_DATA8(0x08); 
	TFT_WR_DATA8(0x04); 
	TFT_WR_REG(0xC1); 
	TFT_WR_DATA8(0XC0); 
	TFT_WR_REG(0xC2); 
	TFT_WR_DATA8(0x0D); 
	TFT_WR_DATA8(0x00); 
	TFT_WR_REG(0xC3); 
	TFT_WR_DATA8(0x8D); 
	TFT_WR_DATA8(0x2A); 
	TFT_WR_REG(0xC4); 
	TFT_WR_DATA8(0x8D); 
	TFT_WR_DATA8(0xEE); 
	//---------------------------------End ST7735S Power Sequence-------------------------------------// 
	TFT_WR_REG(0xC5); //VCOM 
	TFT_WR_DATA8(0x1A); 
	TFT_WR_REG(0x36); //MX, MY, RGB mode 
	if(USE_HORIZONTAL==0){
        TFT_WR_DATA8(0x00);
    }
	else if(USE_HORIZONTAL==1){
        TFT_WR_DATA8(0xC0);
    }
	else if(USE_HORIZONTAL==2){
        TFT_WR_DATA8(0x70);
    }
	else {
        TFT_WR_DATA8(0xA0); 
    }
	//------------------------------------ST7735S Gamma Sequence---------------------------------// 
	TFT_WR_REG(0xE0); 
	TFT_WR_DATA8(0x04); 
	TFT_WR_DATA8(0x22); 
	TFT_WR_DATA8(0x07); 
	TFT_WR_DATA8(0x0A); 
	TFT_WR_DATA8(0x2E); 
	TFT_WR_DATA8(0x30); 
	TFT_WR_DATA8(0x25); 
	TFT_WR_DATA8(0x2A); 
	TFT_WR_DATA8(0x28); 
	TFT_WR_DATA8(0x26); 
	TFT_WR_DATA8(0x2E); 
	TFT_WR_DATA8(0x3A); 
	TFT_WR_DATA8(0x00); 
	TFT_WR_DATA8(0x01); 
	TFT_WR_DATA8(0x03); 
	TFT_WR_DATA8(0x13); 
	TFT_WR_REG(0xE1); 
	TFT_WR_DATA8(0x04); 
	TFT_WR_DATA8(0x16); 
	TFT_WR_DATA8(0x06); 
	TFT_WR_DATA8(0x0D); 
	TFT_WR_DATA8(0x2D); 
	TFT_WR_DATA8(0x26); 
	TFT_WR_DATA8(0x23); 
	TFT_WR_DATA8(0x27); 
	TFT_WR_DATA8(0x27); 
	TFT_WR_DATA8(0x25); 
	TFT_WR_DATA8(0x2D); 
	TFT_WR_DATA8(0x3B); 
	TFT_WR_DATA8(0x00); 
	TFT_WR_DATA8(0x01); 
	TFT_WR_DATA8(0x04); 
	TFT_WR_DATA8(0x13); 
	//------------------------------------End ST7735S Gamma Sequence-----------------------------// 
	TFT_WR_REG(0x3A); //65k mode 
	TFT_WR_DATA8(0x05); 
	TFT_WR_REG(0x29); //Display on   
    
  Init_SPI0_GPIO16();
}

