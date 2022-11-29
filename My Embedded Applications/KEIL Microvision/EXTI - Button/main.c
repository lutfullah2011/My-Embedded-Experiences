#include "stm32f10x.h"                  // Device header
#include "stm32f10x_exti.h"             // Keil::Device:StdPeriph Drivers:EXTI

uint16_t counter=0;

void gpioConfig(){
	GPIO_InitTypeDef GPIOInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //for led
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //for button

	   
   //LED 
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_Out_PP; //output
   GPIOInitStructure.GPIO_Pin= GPIO_Pin_8 |GPIO_Pin_9|GPIO_Pin_10;
   GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(GPIOC,&GPIOInitStructure);
   
   //BUTTON
   GPIOInitStructure.GPIO_Mode=GPIO_Mode_IPD; //input pull down
   GPIOInitStructure.GPIO_Pin=GPIO_Pin_0;
   GPIO_Init(GPIOA,&GPIOInitStructure);
   
}

void extiConfig(){
	EXTI_InitTypeDef extiInitstructure;
	NVIC_InitTypeDef nvicInitstructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//Altenative function
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_Pin_0);
	
	//external interrupt part
	extiInitstructure.EXTI_Line = EXTI_Line0;
	extiInitstructure.EXTI_LineCmd=ENABLE;
	extiInitstructure.EXTI_Mode=EXTI_Mode_Interrupt;
	extiInitstructure.EXTI_Trigger=EXTI_Trigger_Rising;
	
	EXTI_Init(&extiInitstructure);
	
	//NVIC part;
	nvicInitstructure.NVIC_IRQChannel=EXTI0_IRQn; //Kesme sonrasi fonksiyonu çagirma
  nvicInitstructure.NVIC_IRQChannelCmd=ENABLE;
	nvicInitstructure.NVIC_IRQChannelPreemptionPriority=1;
	nvicInitstructure.NVIC_IRQChannelSubPriority=1;
	
	NVIC_Init(&nvicInitstructure);
}

void delay(uint32_t time){
	
		while(time--);
}


//kesme fonksiyonu
void EXTI0_IRQHandler(){
		
	if(EXTI_GetITStatus(EXTI_Line0)!=RESET){  //RESET durumunda degilse;
		counter++;
		for(int i=0; i<4; i++){
         GPIO_SetBits(GPIOC,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);
         delay(3600000);
         GPIO_ResetBits(GPIOC,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);
         delay(3600000);   
      }
	  
		EXTI_ClearITPendingBit(EXTI_Line0); //interrupt tekrar çalismasi için set durumuna alinir.
		
	} 

}


int main(){
	  gpioConfig();
	  extiConfig();
	  int ledArray[3]={GPIO_Pin_8,GPIO_Pin_9,GPIO_Pin_10};
	
		while(1){
				
//			GPIO_SetBits(GPIOC,GPIO_Pin_10);
//			delay(18000);
//			GPIO_ResetBits(GPIOC,GPIO_Pin_10);
			
			
			for(int i=0; i<3; i++ ){
			GPIO_SetBits(GPIOC,ledArray[i]);
			delay(360000);
			GPIO_ResetBits(GPIOC,ledArray[i]);
		  delay(360000);
			
			}
			for(int i=1; i>0; i-- ){
			GPIO_SetBits(GPIOC,ledArray[i]);
			delay(360000);
			GPIO_ResetBits(GPIOC,ledArray[i]);
		  delay(360000);
			
			}
		
		
		
		}
}
