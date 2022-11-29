//#include "stm32f10x.h"                  // Device header
//#include "stm32f10x_dma.h"              // Keil::Device:StdPeriph Drivers:DMA
//#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC
//#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART

//#define V25 1.50

//char testMessage[25]="Lutfullah AYDIN\r\n";
//char gelenVeri=0;

//uint16_t adcValue[2];

//uint16_t oku;

//uint16_t ldrdeger;

//float temp_voltage;
//float temperature;

//void gpioConfig(){
//   
//   GPIO_InitTypeDef GPIOInitStructure;
//   
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ldr, int temp
// 	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//leds
//   
//   
//   //ADC PIN16 CONFIG - Internal Temp, ADC Channel secicem sadece

//   //ADC PIN1 CONFIG - LDR
//   GPIOInitStructure.GPIO_Mode=GPIO_Mode_AIN;
//   GPIOInitStructure.GPIO_Pin=GPIO_Pin_1;
//   GPIO_Init(GPIOA,&GPIOInitStructure);
//	
//	 //TX->PA9
//   GPIOInitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
//   GPIOInitStructure.GPIO_Pin=GPIO_Pin_9;
//   GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//   GPIO_Init(GPIOA,&GPIOInitStructure);
//   
//   //RX->PA10
//   GPIOInitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
//   GPIOInitStructure.GPIO_Pin=GPIO_Pin_10;
//   GPIO_Init(GPIOA,&GPIOInitStructure);
//	
//	 //leds
//   GPIOInitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//   GPIOInitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
//   GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//   GPIO_Init(GPIOC,&GPIOInitStructure);


//}

//void adcConfig(){
//   
//   ADC_InitTypeDef ADCInitStructure;
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
//   
//   ADCInitStructure.ADC_ContinuousConvMode=ENABLE;
//   ADCInitStructure.ADC_DataAlign=ADC_DataAlign_Right;
//   ADCInitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
//   ADCInitStructure.ADC_Mode=ADC_Mode_Independent;
//   ADCInitStructure.ADC_NbrOfChannel=2;
//   ADCInitStructure.ADC_ScanConvMode=ENABLE; //Birden çok okuma yaptigimiz için enable yaptik
//   
//   ADC_Init(ADC1,&ADCInitStructure);
//   ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_55Cycles5);
//   ADC_RegularChannelConfig(ADC1,ADC_Channel_16,2,ADC_SampleTime_239Cycles5);
//   ADC_Cmd(ADC1,ENABLE);
//   ADC_DMACmd(ADC1,ENABLE);
//   ADC_SoftwareStartConvCmd(ADC1,ENABLE);

//}

//void dmaConfig(){
//   
//   DMA_InitTypeDef DMAInitStructure;
//   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
//   
//   DMA_Cmd(DMA1_Channel1,DISABLE); 
//   DMA_DeInit(DMA1_Channel1);				
//   
//   DMAInitStructure.DMA_BufferSize=2;
//   DMAInitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
//   DMAInitStructure.DMA_M2M=DISABLE;
//   DMAInitStructure.DMA_MemoryBaseAddr=(uint32_t)adcValue;
//   DMAInitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Word;
//   DMAInitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
//   DMAInitStructure.DMA_Mode=DMA_Mode_Circular;
//   DMAInitStructure.DMA_PeripheralBaseAddr=(uint32_t)ADC1->DR; // buraya da bak
//   DMAInitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Word;
//   DMAInitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
//   DMAInitStructure.DMA_Priority=DMA_Priority_High;
//   
//   DMA_Init(DMA1_Channel1,&DMAInitStructure);// DMA 1 4-5 CHANNEL RX TX icin, tek ldr adcsi icin dene
//   DMA_Cmd(DMA1_Channel1,ENABLE);
//}

//void usartConfig(){
//   USART_InitTypeDef USARTInitStructure;
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
//   
//   
//   USARTInitStructure.USART_BaudRate=9600;
//   USARTInitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
//   USARTInitStructure.USART_Mode=USART_Mode_Tx |USART_Mode_Rx;
//   USARTInitStructure.USART_Parity=USART_Parity_No;
//   USARTInitStructure.USART_StopBits=USART_StopBits_1;
//   USARTInitStructure.USART_WordLength=USART_WordLength_8b;
//   
//   USART_Init(USART1,&USARTInitStructure);
//   USART_Cmd(USART1,ENABLE);
//	 USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
// 	 USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
//   
//}

//void usartTransmit(char *string){
//   
//   while(*string){
//      while(!(USART1 ->SR & 0x00000040)); // Check Bit 7 TXE: Transmit data register empty?
//      USART_SendData(USART1,*string);
//      *string++;
//   }
//}

//void delay(uint32_t time)
//{
//   while(time--);      
//}

//uint16_t readADC1(){
//   
//   ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_55Cycles5);
//   ADC_SoftwareStartConvCmd(ADC1,ENABLE);
//   
//   while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
//   
//   return ADC_GetConversionValue(ADC1);

//}

//uint16_t readADC2(){
//   
//   ADC_RegularChannelConfig(ADC1,ADC_Channel_16,2,ADC_SampleTime_55Cycles5);
//   ADC_SoftwareStartConvCmd(ADC1,ENABLE);
//   
//   while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
//   
//   return ADC_GetConversionValue(ADC1);

//}

//int main(){
//   gpioConfig();
//   adcConfig();
//   dmaConfig();
//	 usartConfig();

//	while(1){
//		
//		usartTransmit(testMessage);
//    delay(3600000);
//      
//    gelenVeri=USART_ReceiveData(USART1);
//		 
//		oku=adcValue[1];
//		temp_voltage=(float)oku*(3.3/4095);
//    temperature=(V25-temp_voltage)/0.0043+25;   // V25 =1.46 //0.76    //Calculate the current temperature value    //Temperature={(V25-Vsense)/Avg_Slope}+25 ; Avg_Slope=4.3 mV    
//    delay(250);
//		
//		ldrdeger=ADC_GetConversionValue(ADC1);
//		
//		USART_SendData(USART1,(float)adcValue[0]);
//		delay(360000);
//		//USART_SendData(USART1,adcValue[1]);
//      
//		  if(adcValue[0]<2100){
//         GPIO_SetBits(GPIOC,GPIO_Pin_8);
//         GPIO_ResetBits(GPIOC,GPIO_Pin_9 | GPIO_Pin_10);
//      
//      }
//      if(adcValue[0]>=2100 && adcValue[0]<3400){
//         GPIO_SetBits(GPIOC,GPIO_Pin_9);
//         //GPIO_ResetBits(GPIOB,GPIO_Pin_2);
//            GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_10);
//      
//      }
//         if(adcValue[0]>=3400 /*&& adcValue[0]<4095*/){
//         GPIO_SetBits(GPIOC,GPIO_Pin_10);
//         GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_9);
//      }
//   
//   
//   
//   }



//}


#include "stm32f10x.h"                  // Device header
#include "stm32f10x_dma.h"              // Keil::Device:StdPeriph Drivers:DMA
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include <stdio.h>

#define V25 1.53 //V25=Vsense at 25 degrees , 25 derecedeki voltaj degeri(typical value: 1.32 & 1.43 & 1.50).

float temp_voltage=0;
float temperature=0;

uint16_t adcValue[3];
uint16_t okuldr=0;
uint16_t okupot=0;
uint16_t okutemp=0;

char testMessage[45];
char gelenVeri=0;

void gpioConfig(){
   
   GPIO_InitTypeDef GPIOInitStructure;
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);// ldr,pot,int temp
 	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//leds   
   
   //ADC PIN0 CONFIG
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_AIN;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_0;
   GPIO_Init(GPIOA,&GPIOInitStructure);
   
   //ADC PIN1 CONFIG
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_AIN;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_1;
   GPIO_Init(GPIOA,&GPIOInitStructure);

	 //TX->PA9
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_9;
   GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(GPIOA,&GPIOInitStructure);
   
   //RX->PA10
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_10;
   GPIO_Init(GPIOA,&GPIOInitStructure);

	 //leds
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
   GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(GPIOC,&GPIOInitStructure);	 


}

void adcConfig(){
   
   ADC_InitTypeDef ADCInitStructure;
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
   
   ADCInitStructure.ADC_ContinuousConvMode=ENABLE;
   ADCInitStructure.ADC_DataAlign=ADC_DataAlign_Right;
   ADCInitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
   ADCInitStructure.ADC_Mode=ADC_Mode_Independent;
   ADCInitStructure.ADC_NbrOfChannel=3;
   ADCInitStructure.ADC_ScanConvMode=ENABLE; //Birden çok okuma yaptigimiz için enable yaptim
   
   ADC_Init(ADC1,&ADCInitStructure);
   ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5); //POT
   ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_71Cycles5); //LDR
	 ADC_RegularChannelConfig(ADC1,ADC_Channel_16,3,ADC_SampleTime_239Cycles5); //int temp
	 ADC_TempSensorVrefintCmd(ENABLE);// VEYA ADC1->CR2 |= 1<<23;  //Set the TSVREFE bit for internal temperature
   ADC_Cmd(ADC1,ENABLE);
   ADC_DMACmd(ADC1,ENABLE);
	
	 ADC_ResetCalibration(ADC1); //Enable ADC1 reset calibration register	 
	 while(ADC_GetResetCalibrationStatus(ADC1));//Check the end of ADC1 reset calibration register	 
	 ADC_StartCalibration(ADC1);//Start ADC1 calibration	
	 while(ADC_GetCalibrationStatus(ADC1)); //Check the end of ADC1 calibration	
	 
   ADC_SoftwareStartConvCmd(ADC1,ENABLE);

}

void dmaConfig(){
   
   DMA_InitTypeDef DMAInitStructure;
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
   
   DMA_Cmd(DMA1_Channel1,DISABLE);  //çok gerekli deil
   DMA_DeInit(DMA1_Channel1);				//çok gerekli deil
   
   DMAInitStructure.DMA_BufferSize=3;
   DMAInitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
   DMAInitStructure.DMA_M2M=DISABLE;
   DMAInitStructure.DMA_MemoryBaseAddr=(uint32_t)adcValue;
   DMAInitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
   DMAInitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
   DMAInitStructure.DMA_Mode=DMA_Mode_Circular;
   DMAInitStructure.DMA_PeripheralBaseAddr=(uint32_t) & ADC1->DR;//(uint32_t)ADC1_DR
   DMAInitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
   DMAInitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
   DMAInitStructure.DMA_Priority=DMA_Priority_High;
   
   DMA_Init(DMA1_Channel1,&DMAInitStructure);
   DMA_Cmd(DMA1_Channel1,ENABLE);
}

void usartConfig(){
	
   USART_InitTypeDef USARTInitStructure;
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    
   USARTInitStructure.USART_BaudRate=9600;
   USARTInitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
   USARTInitStructure.USART_Mode=USART_Mode_Tx |USART_Mode_Rx;
   USARTInitStructure.USART_Parity=USART_Parity_No;
   USARTInitStructure.USART_StopBits=USART_StopBits_1;
   USARTInitStructure.USART_WordLength=USART_WordLength_8b;
   
   USART_Init(USART1,&USARTInitStructure);
   USART_Cmd(USART1,ENABLE);
	
   
}

void usartTransmit(char *string){
   
   while(*string){
      while(!(USART1 ->SR & 0x00000040)); // Check Bit 7 TXE: Transmit data register empty?
      USART_SendData(USART1,*string);
      *string++;
   }
}

void delay(uint32_t time)
{
   while(time--);      
}


int main(){
   gpioConfig();
   adcConfig();
   dmaConfig();
	 usartConfig();
   
   while(1){
		 
		 	okupot = adcValue[0];
		  delay(36000); 
		  okuldr = adcValue[1];
		  delay(36000);
		 
	  	okutemp=adcValue[2];
		  temp_voltage=(float)okutemp*(3.3/4095);
      temperature=(V25-temp_voltage)/0.0043+25;   // V25 =1.50    
		  //Calculate the current temperature value -->  temp_voltage = Vsense    
		  //Temperature={(V25-Vsense)/Avg_Slope}+25 ; Avg_Slope=4.0 & 4.3 & 4.6 mV    
      delay(36000);		 
		 

		  sprintf(testMessage,"adcValue_POT = %d || adcValue_LDR = %d ||  Temperature = %0.1f\n",okupot,okuldr,temperature);
	  	usartTransmit(testMessage);
      delay(7000000); 
 		
      
      //gelenVeri=USART_ReceiveData(USART1);
		 
		  if(okuldr<2300 && okupot<2300){
         GPIO_SetBits(GPIOC,GPIO_Pin_8);
         GPIO_ResetBits(GPIOC,GPIO_Pin_9 | GPIO_Pin_10);
      
      }
		  else if(okuldr>=2300 && okupot>=2300 && okuldr<3700 && okupot<3700 ){
         GPIO_SetBits(GPIOC,GPIO_Pin_9);
				 GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_10);
      
      }
		  else if(okuldr>=3700 && okupot>=3700){
         GPIO_SetBits(GPIOC,GPIO_Pin_10);
         GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_9);
      }   
			else
				GPIOC->ODR^= 7<<8 ; // 111 << 8 KERE sola shift, 8,9,10. pinler
				delay(360);
   
   
   }



}
