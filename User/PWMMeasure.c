#include "PWMMeasure.h"
#include "stdint.h"
#include "main.h"
#include "adc.h"

uint8_t PWMMearueResult;

uint8_t PWMAnylzing(volatile struct Oscilloscope *value){
	uint16_t num=30;
	uint32_t res=0;
	uint16_t i;
	float adcValue;
	dma_transfer_number_config(DMA_CH0, 300);
  dma_channel_enable(DMA_CH0);
	while(num){
	 //如果获取电压值完成，开始刷屏
        if((*value).showbit==1)
        {           
            (*value).showbit=0;
					
					  (*value).pvpp=0;
					  (*value).nvpp=0;
					
						Opt_ADC_Value((*value).MedFilter,(*value).AvgFilter);
            //转换电压值
            for(i=0;i<300;i++)
            {
								adcValue = (Get_ADC_Value(i)*3.3f)/4096.0f;//0-3.3
							
								(*value).voltageValue[i] = (5-(2.0f*adcValue))* ((*value).dimmerMultpile+0.0f);//true voltage
							

                if(((*value).pvpp) < (*value).voltageValue[i])//find max voltage
                {
                    (*value).pvpp = (*value).voltageValue[i];
                }
								if(((*value).nvpp) > (*value).voltageValue[i])//find max voltage
                {
                    (*value).nvpp = (*value).voltageValue[i];
                }
                
            }
						 //同时获取电压值
            dma_transfer_number_config(DMA_CH0, 300);
            dma_channel_enable(DMA_CH0);
						res+=PWMMeasueOnce(value);
						//printf("%d\n",res);
						num--;
				}	
		}
	
	PWMMearueResult=res/30;
	return PWMMearueResult;
}


uint8_t PWMMeasueOnce(volatile struct Oscilloscope *value){
	uint16_t RisingEdge1=0;
	uint16_t RisingEdge2=0;
	uint16_t FallingEdge1=0;
	uint16_t FallingEdge2=0;
	uint16_t i;
	
	

	float trig = (*value).pvpp+(*value).nvpp;
	for(i=0;i<300;i++)
   {
         if((*value).voltageValue[i] < trig)//finding rising edge
          {
                 for(;i<200;i++)
                   {
                       if((*value).voltageValue[i] > trig)
                        {
                            RisingEdge1=i;
                            break;
                        }
                    }
									 break;
         }
   }
	 for(i=RisingEdge1;i<300;i++)
   {
         if((*value).voltageValue[i] < trig)//finding rising edge
          {
               for(;i<200;i++)
                {
                    if((*value).voltageValue[i] > trig)
                      {
                            RisingEdge2=i;
                            break;
                       }
                 }
								break;
         }
   }
	 for(i=0;i<300;i++)
   {
         if((*value).voltageValue[i] > trig)//finding falling edge
          {
                 for(;i<200;i++)
                   {
                       if((*value).voltageValue[i] < trig)
                        {
                            FallingEdge1=i;
                            break;
                        }
                    }
									 break;
         }
   }
	 for(i=FallingEdge1;i<300;i++)
   {
         if((*value).voltageValue[i] > trig)//finding falling edge
          {
               for(;i<200;i++)
                {
                    if((*value).voltageValue[i] < trig)
                      {
                            FallingEdge2=i;
                            break;
                       }
                 }
								break;
         }
   }
	 uint16_t period=((RisingEdge2-RisingEdge1)+(FallingEdge2-FallingEdge1))/2;
	 if(RisingEdge1<FallingEdge1){
			return (FallingEdge1-RisingEdge1)*100/period;
	 }else{
			return (period-(RisingEdge1-FallingEdge1))*100/period;
	 }
}


