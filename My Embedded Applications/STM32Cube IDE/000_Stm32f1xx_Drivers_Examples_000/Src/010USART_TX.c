/*
 * 010USART_TX.c
 *
 *  Created on: 19 Mar 2023
 *      Author: lutfullah_aydin
 */



/******************************************************************************/
/*                  ##### EXPLANATION OF THIS APPLICATION #####	              */
/******************************************************************************/

/*
 * @This application show the communication between STM32 and ARDIUNO device.
 * @This application transmit logical commands to Arduino(Slave) side in I2C communication with polling(blocking) mode.
 */



#include<stdio.h>
#include<string.h>
#include "stm32f1xx.h"


USART_Handle_t USART1Handle; // Define this handle as Global variable because we will use it:)

//some random data
char msg[1024] = "USART Tx testing...\n\r";

void gpioConfig(){

	GPIO_Handle_t  pGPIOusart, GPIOLed, GPIOButton;


	// USART
	pGPIOusart.pGPIOx = GPIOA;
	pGPIOusart.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_AF_PP; //Push pull configuration for usart bus
	pGPIOusart.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_HIGH;

	//For TX
	pGPIOusart.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(&pGPIOusart);

	//For RX
	pGPIOusart.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(&pGPIOusart);


	//For LED
	GPIOLed.pGPIOx = GPIOB;
	GPIOLed.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_OUT_PP;
	GPIOLed.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_1;
	GPIOLed.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_MEDIUM;
	GPIO_Init(&GPIOLed);

	GPIOLed.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(&GPIOLed);

	//For button
	GPIOButton.pGPIOx = GPIOA;
	GPIOButton.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_INPUT_PD;
	GPIOButton.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(&GPIOButton);

}


/*
 * PA9 -->  USART1_TX
 * PA10 --> USART1_RX
 */
void usartConfig(){

	//Handle function created as global variable.

	USART1Handle.pUSARTx = USART1 ;
	USART1Handle.USARTConfig.USART_Mode = USART_MODE_TX;
	USART1Handle.USARTConfig.USART_Baud = USART_STD_BAUD_115200;
	USART1Handle.USARTConfig.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	USART1Handle.USARTConfig.USART_ParityControl = USART_PARITY_DISABLE;
	USART1Handle.USARTConfig.USART_StopBits = USART_STOPBITS_1;
	USART1Handle.USARTConfig.USART_WordLength = USART_WORDLEN_8BITS;

	USART_Init(&USART1Handle);
}



void delay(uint32_t time){

	while(time--);

}


int main(void)
{

	gpioConfig();  // GPIO INIT
	usartConfig(); // USART INIT

	//Enable the usart peripheral
	USART_PeriphControl(USART1,ENABLE);

	while(1)
	{
		delay(500000);
		GPIO_ToggleOutputPin(GPIOB, GPIO_Pin_1);

		while ( ! (GPIO_ReadFromInputPin(GPIOA, GPIO_Pin_0)) ); // Data will send only if I press the button. Because code will not hanging in "here (while)" when I press the button

		delay(500000); //small delay for button debouncing issues.


		//sending message
		USART_SendData(&USART1Handle, (uint8_t*)msg,strlen(msg));
	}

}
