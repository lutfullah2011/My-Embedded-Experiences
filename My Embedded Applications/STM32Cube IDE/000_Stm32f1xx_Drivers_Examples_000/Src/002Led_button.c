/*
 * 002Led_button.c
 *
 *  Created on: 22 Kas 2022
 *      Author: luty1
 */


#include <stdint.h>
#include "stm32f1xx.h"

void gpioConfig(){

	GPIO_Handle_t  GPIOLed, GPIOButton;

	//For LED
	GPIOLed.pGPIOx = GPIOC;
	GPIOLed.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_OUT_PP;
	GPIOLed.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_10;
	GPIOLed.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_MEDIUM;
	GPIO_PCLK(GPIOC, ENABLE);
	GPIO_Init(&GPIOLed);

	//For button
	GPIOButton.pGPIOx = GPIOA;
	GPIOButton.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_INPUT_PD;
	GPIOButton.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_0;
	GPIO_PCLK(GPIOA, ENABLE);
	GPIO_Init(&GPIOButton);

}

void delay(uint32_t time){

	while(time--);

}

int main(void)
{
	gpioConfig();
	uint32_t button_state=0;
	//uint32_t flag = 0;

	while(1){


		button_state=GPIO_ReadFromInputPin(GPIOA, GPIO_Pin_0);

		if(button_state)
		{
			delay(500000);
			GPIO_ToggleOutputPin(GPIOC, GPIO_Pin_10);
		}

//		while(GPIO_ReadFromInputPin(GPIOA,GPIO_Pin_10)!=0);

//      if(button_state==1){
//
//         if(flag==0){
//            flag=1;
//						GPIO_WriteToOutputPin(GPIOC, GPIO_Pin_10,GPIO_PIN_SET);
//
//
//
//         }else{
//            flag=0;
//						GPIO_WriteToOutputPin(GPIOC, GPIO_Pin_10,GPIO_PIN_RESET);
//
//
//
//         }
//
//         delay(50000);
//
//
//
//      }



	}

}
