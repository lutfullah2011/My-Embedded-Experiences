#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC

uint16_t adcValue=0;
float mapValue=0;

void gpioConfig(){
   GPIO_InitTypeDef GPIOInitStructure;
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//leds
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ldr
   
   //leds
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
   GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(GPIOC,&GPIOInitStructure);
   
   //LDR
   //GPIOA->CRL &= ~(15<<4);// ~(0000 1111) 4 kere shift
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_AIN;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_1;
   GPIO_Init(GPIOA,&GPIOInitStructure);
}

void adcConfig(){
   ADC_InitTypeDef ADCInitStructure;
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
   
   ADCInitStructure.ADC_ContinuousConvMode=ENABLE;
   ADCInitStructure.ADC_DataAlign=ADC_DataAlign_Right;
   ADCInitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
   ADCInitStructure.ADC_Mode=ADC_Mode_Independent;
   ADCInitStructure.ADC_NbrOfChannel=1;
   ADCInitStructure.ADC_ScanConvMode=DISABLE;
   
   ADC_Init(ADC1,&ADCInitStructure);
   ADC_Cmd(ADC1,ENABLE);

}

uint16_t readADC(){
   
   ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_55Cycles5);
   ADC_SoftwareStartConvCmd(ADC1,ENABLE);
   
   while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
   
   return ADC_GetConversionValue(ADC1);

}

float map(float adcValue, float max, float min, float conMax, float conMin){
   
   return adcValue*((conMax-conMin)/(max-min));

}


int main(){
   gpioConfig();
   adcConfig();
	
   while(1){
      adcValue=readADC();
      mapValue=map(adcValue,4030,0,900,0);
		 	


      if(mapValue>=0 && mapValue<300){
         GPIO_SetBits(GPIOC,GPIO_Pin_8);
         GPIO_ResetBits(GPIOC,GPIO_Pin_9 | GPIO_Pin_10);
      
      }
      if(mapValue>=300 && mapValue<750){
         GPIO_SetBits(GPIOC,GPIO_Pin_9);
         //GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			   GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_10);
      
      }
         if(mapValue>=750 && mapValue<900){
         GPIO_SetBits(GPIOC,GPIO_Pin_10);
				 GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_9);
      }
//				 else{
//				 GPIO_ResetBits(GPIOB,GPIO_Pin_0 |GPIO_Pin_1 | GPIO_Pin_2);

//				 }

   }




}


//#include "stm32f10x.h"                  // Device header

//void gpioConfig(){
//	GPIO_InitTypeDef gpioInitstructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	
//	gpioInitstructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	gpioInitstructure.GPIO_Pin=GPIO_Pin_0;
//	gpioInitstructure.GPIO_Speed=GPIO_Speed_50MHz;
//	
//	GPIO_Init(GPIOA,&gpioInitstructure);
//	

//}


//void delay(uint32_t time){
//		while(time--);
//}


//int main(){
//	gpioConfig();
//	
//	while(1){
//		
//      GPIO_SetBits(GPIOA,GPIO_Pin_0);
//      for(int i=0;i<360000;i++);
//      GPIO_ResetBits(GPIOA,GPIO_Pin_0);
//      for(int i=0;i<720000;i++);
//      GPIO_SetBits(GPIOA,GPIO_Pin_0);
//      for(int i=0;i<720000;i++);
//      GPIO_ResetBits(GPIOA,GPIO_Pin_0);
//      for(int i=0;i<360000;i++);
//      GPIO_SetBits(GPIOA,GPIO_Pin_0);
//      for(int i=0;i<1080000;i++);
//      GPIO_ResetBits(GPIOA,GPIO_Pin_0);
//      for(int i=0;i<2160000;i++);
//      GPIO_SetBits(GPIOA,GPIO_Pin_0);
//      for(int i=0;i<216000;i++);
//      GPIO_ResetBits(GPIOA,GPIO_Pin_0);
//      for(int i=0;i<360000;i++);
//      
//		
//	}
//}
