#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC
#include <stdio.h>

/*
@@@@@@  Application of 3 Channel ADC conversion without DMA or Interrupt  @@@@@@

Using polling method in while loop, first I could not take the values from ADC1->DR  
Using scan mode is prevent me to take seperate different channel data. 
So I take the each data after I configure channel and sequence infos and put them different variables(okupot,okuldr,okupot). 
The trick is the number of conversion sets to 1 despite I want to read 3.
That was the simple explanation I think

DMA kullanimi disinda degerleri almanin mümkün olmadigini gördüm. O yüzden ADC yi daha iyi anlamak ve kendimi denemek icin bu uygulamayi yaptim.
3 ADC OKUMASI = POT,LDR,INTERNAL TEMP. Degerleri okuyup USART ile virtual terminal de görüntüledim.

Belki Discontinious mode ile de olabilir, denemedim.

*/




const float V25 = 1.52;	//V25=Vsense at 25 degrees , 25 derecedeki voltaj degeri(typical value: 1.32 & 1.43 & 1.50).
const float Avg_slope = 0.0043; //When avg_slope=4.3mV/C at ref 3.3V

float temp_voltage=0;
float temperature=0;

uint16_t okuldr=0;
uint16_t okupot=0;
uint16_t okutemp=0;

char testMessage[45];

void gpioConfig(){
	
   GPIO_InitTypeDef GPIOInitStructure;
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//leds
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//pot, ldr, int temp
   
	 //leds
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
   GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(GPIOC,&GPIOInitStructure);

	 //ADC PIN0,PIN1 CONFIG - POT,LDR
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_AIN;
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1;
   GPIO_Init(GPIOA,&GPIOInitStructure);   	
	
	 //ADC INTERNAL PIN CONFIG - Internal Temp, ADC Channel secicem sadece

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
      USART_SendData(USART1,*string);					// This bit is used during single buffer transmission.(TXE bit)
      *string++;
   }
}


void adcConfig()
{ 
   ADC_InitTypeDef ADCInitStructure;
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
   
   ADCInitStructure.ADC_ContinuousConvMode=DISABLE; //Olup olmamasi çok önemli degil, ikisindede farki anlayamadim
   ADCInitStructure.ADC_DataAlign=ADC_DataAlign_Right;
   ADCInitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
   ADCInitStructure.ADC_Mode=ADC_Mode_Independent;  //neden dual yok? anlamadim,ref manuelde yaziyor sanki
	 ADCInitStructure.ADC_NbrOfChannel=1; // TRICK:))) 3 degil 1 channel okuyormusuz gibi
	 ADCInitStructure.ADC_ScanConvMode=ENABLE; //Birden çok okuma yaptigimiz için enable yaptim, TRICK kullanirken disable yapincada oldu,olmamasi lazim diye düsünmüstüm:)
   
   ADC_Init(ADC1,&ADCInitStructure);
		 
 	 ADC_TempSensorVrefintCmd(ENABLE);
   ADC_Cmd(ADC1,ENABLE);
//	 ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
	 
	 ADC_ResetCalibration(ADC1); //Enable ADC1 reset calibration register	 
	 while(ADC_GetResetCalibrationStatus(ADC1));//Check the end of ADC1 reset calibration register	 
	 ADC_StartCalibration(ADC1);//Start ADC1 calibration	
	 while(ADC_GetCalibrationStatus(ADC1)); //Check the end of ADC1 calibration
	 
	 /* Bunu burda aktif edince hangi kanali okuyacagini sasiriyor veya ben tam anlamadim:)*/
	 //ADC_SoftwareStartConvCmd(ADC1,ENABLE); //Yazilimsal olarak tetiklemeyi baslattim.
}

//void NVICConfig()
//{
//	 NVIC_InitTypeDef NVIC_InitStructure;
//	
//	 /* Enable the ADC IRQ Channel */
//	 NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
//	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	 NVIC_Init(&NVIC_InitStructure);
//}


//void ADC1_IRQHandler()
//{
//	okuldr = ADC_GetConversionValue(ADC1);
//	ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);	
//}


/* Hepsinin ranki 1 secilecek cunku hepsini ayri ayri okucaz ve her ayri okumada öncelik hep okudugumuz kanalin olucak */
/* Reference Manuel syf 162 ->> If the ADC_SQRx or ADC_JSQR registers are modified during a conversion, the current
conversion is reset and a new start pulse is sent to the ADC to convert the new chosen
group.*/

void	readADCpot()
{   
   ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_28Cycles5);
	 ADC_SoftwareStartConvCmd(ADC1,ENABLE);
 	 while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);  
	 okupot = ADC_GetConversionValue(ADC1);
	 ADC_SoftwareStartConvCmd(ADC1,DISABLE);
}


void  readADCldr()
{  
   ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_41Cycles5);
	 ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	 while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);   
	 okuldr = ADC_GetConversionValue(ADC1);
	 ADC_SoftwareStartConvCmd(ADC1,DISABLE);
}

void readADCtemp()
{   
   ADC_RegularChannelConfig(ADC1,ADC_Channel_16,1,ADC_SampleTime_55Cycles5); // Tconv = 17 us olmasi icin -> Tcon = (Sampling time +12.5 cycle)/ADC CLOCK(4 MHZ -> 8/2) ->Sampling time=55.5
	 ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	 while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);   
	 okutemp	= ADC_GetConversionValue(ADC1);
	 ADC_SoftwareStartConvCmd(ADC1,DISABLE);
}




void delay(uint32_t time)
{
   while(time--);      
}


int main(){
   gpioConfig();
   adcConfig();
	 usartConfig();
	 //NVICConfig();
   
   while(1){
	 
		  readADCtemp(); 
			delay(4000);
			readADCpot(); 
		  delay(4000);
		  readADCldr();
		  delay(4000);	
			
			 
			temp_voltage=(float)okutemp*(3.3/4095);
      temperature=(V25-temp_voltage)/Avg_slope+25;   // V25 = 1.50  , Avg_slope = 0.0043  
		  //Calculate the current temperature value -->  temp_voltage = Vsense    
		  //Temperature={(V25-Vsense)/Avg_Slope}+25 ; Avg_Slope=4.0 & 4.3 & 4.6 mV    
      delay(400);	

				
			sprintf(testMessage,"POT = %d || LDR = %d || TEMP = %d ||  Temperature = %0.1f \r\n",okupot,okuldr,okutemp,temperature);
	  	usartTransmit(testMessage);
      delay(700000); 

		 
		  if(okuldr<1500){
         GPIO_SetBits(GPIOC,GPIO_Pin_8);
         GPIO_ResetBits(GPIOC,GPIO_Pin_9 | GPIO_Pin_10);
      
      }
      else if(okuldr>=1500 && okuldr<3000){
         GPIO_SetBits(GPIOC,GPIO_Pin_9);
				 GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_10);
      
      }
      else if(okuldr>=3000 ){
         GPIO_SetBits(GPIOC,GPIO_Pin_10);
         GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_9);
      }
      else{
				 GPIO_ResetBits(GPIOB,GPIO_Pin_0 |GPIO_Pin_1 | GPIO_Pin_2);

      }

   }




}
