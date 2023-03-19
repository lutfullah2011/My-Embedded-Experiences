/*
 * 008I2C_Master_Tx_Test.c
 *
 *  Created on: 24 Şub 2023
 *      Author: lutfullah_aydin
 */

/******************************************************************************/
/*                  ##### EXPLANATION OF THIS APPLICATION #####	              */
/******************************************************************************/

/*
 * @This application show the communication between I2C Master(STM32) and I2C Slave(ARDIUNO) device.
 * @This application transmit logical commands to Arduino(Slave) side in I2C communication with polling(blocking) mode.
 */



#include<stdio.h>
#include<string.h>
#include "stm32f1xx.h"

#define MY_ADDR 0x61;

#define SLAVE_ADDR  0x68 //Arduino board



I2C_Handle_t I2C1Handle; // Define this handle as Global variable because we will use it:)

//some random data
uint8_t random_data[] = "Testing I2C master Tx\n"; // Remember that Ardiuno codes written in wire Library and there is limitation of 32 bytes of data at a time


void gpioConfig(){

	GPIO_Handle_t  pGPIOi2c, GPIOLed, GPIOButton;


	/* External 4.7k pull-up resistors are used */
	// I2C1
	pGPIOi2c.pGPIOx = GPIOB;
	pGPIOi2c.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_AF_OD; //Open drain configuration for I2C buses
	pGPIOi2c.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_HIGH;

	//For SCL
	pGPIOi2c.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(&pGPIOi2c);

	//For SDA
	pGPIOi2c.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(&pGPIOi2c);


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
 * PA6 --> I2C1_SCL - Alternate Function Open Drain
 * PA7 --> I2C1_SDA - Alternate Function Open Drain
 */
void i2cConfig(){

	//Handle function created as global variable.

	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2CConfig.I2C_ACK = I2C_ACK_ENABLE;
	I2C1Handle.I2CConfig.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2CConfig.I2C_FastModeDutyCycle = I2C_Fast_Mode_DUTY_2 ; // Since we are not using Fast mode, I will keep whatever I want, dont mind it:))
	I2C1Handle.I2CConfig.I2C_SCLSpeed = I2C_SCL_SPEED_STD;


	I2C_Init(&I2C1Handle);
}



void delay(uint32_t time){

	while(time--);

}


int main(void)
{

	gpioConfig(); // GPIO INIT
	i2cConfig(); // I2C INIT

	//Enable the i2c peripheral
	I2C_PeriphControl(I2C1,ENABLE);

	while(1)
	{
		delay(500000);
		GPIO_ToggleOutputPin(GPIOB, GPIO_Pin_1);

		while ( ! (GPIO_ReadFromInputPin(GPIOA, GPIO_Pin_0)) ); // Data will send only if I press the button. Because code will not hanging in "here (while)" when I press the button

		delay(500000); //small delay for button debouncing issues.


		//send some random data to the slave
		I2C_MasterSendData(&I2C1Handle,random_data,strlen((char*)random_data),SLAVE_ADDR,0);
	}

}
