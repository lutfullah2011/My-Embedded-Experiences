/*
 * 003Button_interrupt.c
 *
 *  Created on: 7 Ara 2022
 *      Author: luty1
 */



#include <stdint.h>
#include <string.h>
#include "stm32f1xx.h"

uint16_t counter=0; // Created for counting the number of interrrupts. For debugging

void gpioConfig(){

	GPIO_Handle_t  GPIOLed, GPIOButton;

	memset(&GPIOLed, 0, sizeof(GPIOLed)); //Structure memory will be cleared for "clear coding"
	memset(&GPIOButton, 0, sizeof(GPIOButton)); //Structure memory will be cleared for "clear coding"

	//For LED
	GPIOLed.pGPIOx = GPIOC;
	GPIOLed.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_OUT_PP;
	GPIOLed.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_10;
	GPIOLed.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_MEDIUM;
	GPIO_PCLK(GPIOC, ENABLE);
	GPIO_Init(&GPIOLed);

	//For button
	GPIOButton.pGPIOx = GPIOA;
	GPIOButton.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_IT_FT;
	GPIOButton.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_1;
	GPIO_PCLK(GPIOA, ENABLE);
	GPIO_Init(&GPIOButton);

}


void delay(uint32_t time){

	while(time--);

}


void EXTI1_IRQHandler()
{

	counter++;
	for(int i=0; i<6; i++)
	{
		GPIO_ToggleOutputPin(GPIOC, GPIO_Pin_10);
		delay(200000);
	}

	GPIO_IRQHandling(GPIO_Pin_1); // Clear the pending register from EXTI line
}




int main(void)
{
	gpioConfig();

	//IRQ Configuration
	GPIO_IRQPriorityConfig(IRQn_EXTI1, NVIC_IRQ_PRI0);
	GPIO_IRQInterruptConfig(IRQn_EXTI1, ENABLE);



	while(1){

		GPIO_WriteToOutputPin(GPIOC, GPIO_Pin_10, GPIO_PIN_SET);


	}

}
