#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC

//Yari REGISTER yari std library kullanarak internal temp okuma Uygulamasi

//The conversion range is between 2 V < VDDA < 3.6 V.
 
#define V25 1.50 //V25=Vsense at 25 degrees , 25 derecedeki voltaj degeri(typical value: 1.32 & 1.43 & 1.50).
uint16_t oku;
float temp_voltage;
float temperature; //temperature={(V25-Vsense)/Avg_Slope}+25 ;

void gpioConfig()
{
   GPIO_InitTypeDef gpio_Initstruct;
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//adc hatti 
   
   gpio_Initstruct.GPIO_Mode=GPIO_Mode_Out_PP;
   gpio_Initstruct.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
   gpio_Initstruct.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(GPIOC,&gpio_Initstruct);
   
    // Internal Temperature Sensor GPIO Init, PIN NEDEN SECEYIM KI , SECMICEM,ADC CHANNEL SECICEM SADECE
   //GPIOA->CRH &= ~(15<<28);// ~(0000 1111) 28 kere shift,16. channel ,gpio_Initstruct.GPIO_Mode=GPIO_Mode_AIN;
   gpio_Initstruct.GPIO_Pin=GPIO_Pin_1; //   ADC1_IN16;
   GPIO_Init(GPIOA,&gpio_Initstruct);
   
}


void delay(uint32_t time)
{
   while(time--);      
}

void adcConfig()
{
    ADC_InitTypeDef ADCInitStructure;
   
   RCC->APB2ENR |= (1<<9); // 9. biti set ettim. ADC1 Clock aktif:) //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);   
   
   ADCInitStructure.ADC_ContinuousConvMode=ENABLE;
   ADC1->CR2 &= ~(1<<11);   // CR2 align 11. bit 0.   ADCInitStructure.ADC_DataAlign=ADC_DataAlign_Right; //ADCInitStructure.ADC_DataAlign=ADC_DataAlign_Right;
   ADCInitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
   ADCInitStructure.ADC_Mode=ADC_Mode_Independent;
   ADCInitStructure.ADC_NbrOfChannel=1;
   ADCInitStructure.ADC_ScanConvMode=DISABLE;
   
   ADC_Init(ADC1,&ADCInitStructure);
   ADC1->CR2 |= 1; // Debug ederken Zaten aktif olmus olarak gördüm. ADON biti enabled == ADC_Cmd(ADC1,ENABLE);
   delay(10); //conversion baslatmak icin 1.17 us civari beklenir
   
	 //ADC1->SQR1 |= (1<<20);  //  2 CHANNEL okumasi yapacagimiz icin 0001: 2 conversions

   
   ADC1->CR2 |= (1<<23);//23. BIT enabled,temperature sensor //ADC_TempSensorVrefintCmd(ENABLE);    //bu dogrumu bilmiyorum = ADC_CR2_TSVREFE |= (1<<23);
   
}

uint16_t readADC()
{
   ADC1->SMPR1 |= (7<<18);// Channel16 sample time set 71.5 CYCLE(110) in the 18-19-20 bits. it says 17.1 us is enough Tconv= (Sampling+12.5 cycle)/ADC Clock -> Tconv= (71.5+12.5)/4 = 21us oluyor.
   ADC1->SQR3  |= (((uint8_t)0x10)<<0); //select channel 16 for ADC1 //(0001 0000<<0) veya (16<<0), rank 1 icin 0 kere shift ettik.
   //ADC_RegularChannelConfig(ADC1,ADC_Channel_16,1,ADC_SampleTime_239Cycles5);
   
   
   ADC1->CR2 |= (1<<22);//"Start conversion of regular channels" & "External trigger conversion mode for regular channels" == "0000 0101" shifted 22 times  ADC_SoftwareStartConvCmd(ADC1,ENABLE);
   ADC1->CR2 |= (1<<20); //
  //ADC_SoftwareStartConvCmd(ADC1,ENABLE);
   
   while(!(ADC1->SR & 2));// 0 dan farkli her deger TRUE döner.  SR registerin 2. biti end of conversion biti 1 mi kontrol ediliyor. 
   //while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
   
   return (uint16_t) ADC1->DR;// 
   //return ADC_GetConversionValue(ADC1); 

}



//Avg_Slope=the average slope of temperature versus Vsense curve (in mv/C or uv/C) (typical value is 4.3Mv/C).

//With the above formula, we can easily calculate the temperature of the current temperature sensor.



int main()
{
   gpioConfig();
   adcConfig();
   
   while(1){
      
	  	oku=readADC();
		  temp_voltage=(float)oku*(3.3/4095);
      temperature=(V25-temp_voltage)/0.0043+25;   // V25 =1.50    
		  //Calculate the current temperature value -->  temp_voltage = Vsense    
		  //Temperature={(V25-Vsense)/Avg_Slope}+25 ; Avg_Slope=4.0 & 4.3 & 4.6 mV    
      delay(250);
      
      if(temperature>=15 && temperature<25){
         GPIO_SetBits(GPIOC,GPIO_Pin_8);
         GPIO_ResetBits(GPIOC,GPIO_Pin_9 | GPIO_Pin_10);
      
      }
      else if(temperature>=25 && temperature<35){
         GPIO_SetBits(GPIOC,GPIO_Pin_9);
         //GPIO_ResetBits(GPIOB,GPIO_Pin_2);
          GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_10);
      
      }
      else if(temperature>=35 && temperature<50){
         GPIO_SetBits(GPIOC,GPIO_Pin_10);
         GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_9);
      }
      else   {
          GPIO_SetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_9|GPIO_Pin_10);
//          delay(25000000);
//          GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_9|GPIO_Pin_10); 
       }
   }
   
}
