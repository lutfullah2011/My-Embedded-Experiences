#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include "stdio.h"

/*ADC Interrupt Uygulamasi, USARTLA verileri terminale aktarma*/

char testMessage[25];
volatile u16 counter=0;//Kesme de degisebilcek bir global variable oldugu icin volatile kulladim.
u16 adcValue=0; // typedef uint16_t u16; in header file:))
//float mapValue=0;

void gpioConfig(){
   GPIO_InitTypeDef GPIOInitStructure;
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//leds
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//pot
   
   //leds
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
   GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(GPIOC,&GPIOInitStructure);
   
   //pot
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_AIN;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_0;
   GPIO_Init(GPIOA,&GPIOInitStructure);
	
	 //USART1 TX->PA9
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_9;
   GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(GPIOA,&GPIOInitStructure);
   
   //USART1 RX->PA10
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_10;
   GPIO_Init(GPIOA,&GPIOInitStructure);	
	
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

void NVICConfig()
{
	 NVIC_InitTypeDef NVIC_InitStructure;
	
	 /* Enable the ADC IRQ Channel */
	 NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
}


void ADC1_IRQHandler()
{	
	//if(ADC_GetITStatus(ADC1,ADC_IT_EOC)){ // Kesmenin nedeni end of conversion mu kontrol ediliyor.
		adcValue = ADC_GetConversionValue(ADC1);
		counter++; //kesmeye giriyor mu diye kontrol etmek icin koydum.
	//ADC_ClearITPendingBit(ADC1,ADC_IT_EOC); //buna gerek varmi gerçekten? otomatikman sifirlaniyor mu?	
	//}																				//Gerek yok cünkü Data register okuyunca otomatik sifirlaniy
}

void delay(uint32_t time)
{
   while(time--);      
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
   ADC_Cmd(ADC1,ENABLE); /* reference mauel syf 177 -> If any other bit in this register apart from ADON is 
	 changed at the same time, then conversion is not triggered. This is to prevent triggering an erroneous conversion.*/
	 ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
	 ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	 ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);	

	 ADC_ResetCalibration(ADC1); //Enable ADC1 reset calibration register	 
	 while(ADC_GetResetCalibrationStatus(ADC1));//Check the end of ADC1 reset calibration register	 
	 ADC_StartCalibration(ADC1);//Start ADC1 calibration	
	 while(ADC_GetCalibrationStatus(ADC1)); //Check the end of ADC1 calibration
}


int main(){
   gpioConfig();
	 usartConfig();
	 adcConfig();
	 NVICConfig();

   while(1){
		 
		 
		 	sprintf(testMessage,"POT_adcValue = %d || counter = %d \r\n",adcValue,counter);
	  	usartTransmit(testMessage);
      delay(100000); 
		 	


      if(/*adcValue>=0 && */adcValue<1350){
         GPIO_SetBits(GPIOC,GPIO_Pin_8);
         GPIO_ResetBits(GPIOC,GPIO_Pin_9 | GPIO_Pin_10);
      
      }
      else if(adcValue>=1350 && adcValue<2900){
         GPIO_SetBits(GPIOC,GPIO_Pin_9);
			   GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_10);
      
      }
      else if(adcValue>=2900 && adcValue<4095){
         GPIO_SetBits(GPIOC,GPIO_Pin_10);
				 GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_9);
      }
			else{
				GPIOC->ODR^= 7<<8 ; // 111 << 8 KERE sola shift, 8,9,10. pinler
				delay(36000);
				
				 }

   }




}
