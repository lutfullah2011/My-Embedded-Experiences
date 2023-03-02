/*
 * 009I2C_Slave_Tx_InterruptMode_Test.c
 *
 *  Created on: 2 Mar 2023
 *      Author: luty1
 */


/******************************************************************************/
/*                  ##### EXPLANATION OF THIS APPLICATION #####	              */
/******************************************************************************/

/*
 * @This application show the communication between I2C Master(ARDIUNO) and I2C Slave(STM32) device.
 * @This application transmit some string data to Arduino(Master) side in I2C communication with interrupt mode
 * @ The codes for Master(for arduino) will provided from the Mastering MCU course
 */

#include<stdio.h>
#include<string.h>
#include "stm32f1xx.h"

#define SLAVE_ADDR  0x69 //STM32 board

#define MY_ADDR SLAVE_ADDR;


I2C_Handle_t I2C1Handle; // Define this handle as Global variable because we will use it:)

//some random data for Tx operation
uint8_t random_data[32] = "Testing I2C slave mode Tx...\n";


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

	//I2C IRQ configurations
	I2C_IRQInterruptConfig(IRQn_I2C1_EV,ENABLE);
	I2C_IRQInterruptConfig(IRQn_I2C1_ER,ENABLE);

	I2C_SlaveEnableDisableCallbackEvents(I2C1,ENABLE);

	//Enable the i2c peripheral
	I2C_PeriphControl(I2C1,ENABLE);

	//ack bit is made 1 after PE=1, this is actually handled in I2C_Init API but, we need to do it after Peripheral enable.
	I2C_ManageAcking(I2C1,I2C_ACK_ENABLE);

	while(1)
	{
		//Hangs here
	}

}





// Runs when an event occurs from the peripheral over I2C and fires to I2C_ApplicationEventCallback
void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}

// Runs when an error occurs from the peripheral over I2C and fires to I2C_ApplicationEventCallback
void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&I2C1Handle);
}


void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv)
{

	static uint8_t commandCode = 0; // These variables are static. So, the values of them will not change every iteration.
	static  uint8_t Counter = 0;    // Allocated in Global space, not in stacks. You can not access these variables outside this function scope.

	if(AppEv == I2C_EV_DATA_REQ) // Second, this code will be executed
	{
		//Master wants some data. slave has to send it
		if(commandCode == 0x51)
		{
			//send the length information to the master
			I2C_SlaveSendData(pI2CHandle->pI2Cx,strlen((char*)random_data));
		}else if (commandCode == 0x52)
		{
			//Send the contents of random_data[] byte by byte every time
			I2C_SlaveSendData(pI2CHandle->pI2Cx,random_data[Counter++]);

		}
	}else if (AppEv == I2C_EV_DATA_RCV) //First, this code will be executed
	{
		//Data is waiting for the slave to read . slave has to read it
		commandCode = I2C_SlaveReceiveData(pI2CHandle->pI2Cx);

	}else if (AppEv == I2C_ERROR_AF) // Third, this code will be executed
	{
		//This happens only during slave transmition .
		//If this error happens, It means that Master has sent the NACK. so slave should understand that master doesnt need more data.
		commandCode = 0xFF;
		Counter = 0;
	}
	else if (AppEv == I2C_EV_STOPF)
	{
		//This happens only during slave reception.
		//Master has ended the I2C communication with the slave.
	}

}




