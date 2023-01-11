/*
 * 007SPI_Rx_message_from_Ardiuno.c
 *
 *  Created on: 8 Oca 2023
 *      Author: lutfullah_aydin
 */

/******************************************************************************/
/*                  ##### EXPLANATION OF THIS APPLICATION #####	              */
/******************************************************************************/

/*

  @This application receives the user message received from the Arduino side in SPI communication with interrupt mode
  @User sends the message through Arduino IDE's serial monitor and Master will lights its LED according to message.
  @Ardiuno board's pin number 8 connect with the Stm board's PORTB, pin number 0. Hence, whenever Ardiuno wants to
  send messages, it will trigger the Stm board's pin so that interrupt accured.
  @If the message same what we see, toogle the PortB pin number 2. (Message is "nice")

  @This program basically shows how reception is going on. It could be improved for further usage.

  ***Follow the instructions to test this code***

  1. Download this code on to STM32 board , acts as Master
  2. Download Slave code (003SPISlaveUartReadOverSPI.ino) on to Arduino board (Slave)
   (This file available in Github of niekiran/MasteringMCU )
  3. Open Arduino IDE serial monitor and send the messages


*/


#include "stm32f1xx.h"
#include <string.h>
#include <stdio.h>

SPI_Handle_t SPI1handle; // Define this handle as Global variable because we will use it:)

#define MAX_LEN 500

char RcvBuff[MAX_LEN];

volatile char ReadByte;

volatile uint8_t rcvStop = 0;

/*This flag will be set in the interrupt handler of the Arduino, Here is connected to pin 0 of port B */
volatile uint8_t dataAvailable = 0;


void gpioConfig(){

	GPIO_Handle_t  pGPIOspi, GPIOLed;

	//For SPI1
	/*
	 * PA4 --> SPI1_NCC - General Output
	 * PA5 --> SPI1_SCK - Alternate Output PP
	 * PA6 --> SPI1_MISO - Alternate Output PP
	 * PA7 --> SPI1_MOSI - Alternate Output PP
	 */
	pGPIOspi.pGPIOx = GPIOA;
	pGPIOspi.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_AF_PP;
	pGPIOspi.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_HIGH;

	//For SCLK
	pGPIOspi.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(&pGPIOspi);

	//For MISO
	pGPIOspi.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(&pGPIOspi);

	//For MOSI
	pGPIOspi.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(&pGPIOspi);

	//For NCC
	pGPIOspi.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(&pGPIOspi);


	//For LED
	GPIOLed.pGPIOx = GPIOB;
	GPIOLed.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_OUT_PP;
	GPIOLed.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_MEDIUM;

	GPIOLed.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(&GPIOLed);

	GPIOLed.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(&GPIOLed);
}



void spiConfig(){

	SPI1handle.pSPIx = SPI1;
	SPI1handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI1handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI1handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI1handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV32;// fclock generates 8Mhz and  sclk is now mhz. High speed may corrupt some data while communicating with Slave device
	SPI1handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI1handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI1handle.SPIConfig.SPI_SPI_SSM = SPI_SSM_DIS; //Software slave managment disabled, we will use hardware slave managment


	SPI_Init(&SPI1handle);
}



void delay(uint32_t time){

	while(time--);

}



/*This function configures the gpio pin over which SPI peripheral issues data available interrupt */
void GPIO_InterruptPinInit(void)
{
	GPIO_Handle_t IsrPin;
	memset(&IsrPin,0,sizeof(IsrPin));

	//this is led gpio configuration
	IsrPin.pGPIOx = GPIOB;
	IsrPin.GPIO_PinConfig.GPIO_Pin   = GPIO_Pin_0;
	IsrPin.GPIO_PinConfig.GPIO_Mode  = GPIO_MODE_IT_FT;
	IsrPin.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_MEDIUM;
	GPIO_Init(&IsrPin);

	GPIO_IRQPriorityConfig(IRQn_EXTI0,NVIC_IRQ_PRI0);
	GPIO_IRQInterruptConfig(IRQn_EXTI0, ENABLE);

}






int main(void)
{
	uint8_t dummy = 0xFF;

	GPIO_InterruptPinInit();

	gpioConfig(); // GPIO INIT
	spiConfig();  // SPI INIT

	/*
	 * SSOE = 1 means NSS output enable.
	 * NSS pin is managed by hardware.
	 * When spi enabled, NSS will pulled to low.
	 * When spi disabled, NSS will pulled to high.
	 */
	SPI_SSOEConfig(SPI1, ENABLE); // Setting SSOE pin, NSS pin will pull to low in order to activate slave device(Ardiuno in this scenario)

	SPI_IRQInterruptConfig(IRQn_SPI1,ENABLE);

	while (1)
	{

		GPIO_ToggleOutputPin(GPIOB, GPIO_Pin_1);

		rcvStop = 0;

		while (!dataAvailable); //Wait till data available interrupt(EXTI) from Ardiuno. Then Master get notify that I am getting messages
		GPIO_IRQInterruptConfig(IRQn_EXTI0, DISABLE); //Disable it for further interrupt to get only 1 data.


		//Enable the SPI Peripheral
		SPI_PeriphControl(SPI1, ENABLE);

		while(!rcvStop) // Its a flag, indicates when to stop.
		{
			// Get the data from the SPI1 byte by byte in interrupt mode
			//We have to send 1 byte data to receive 1 byte of data. Its the logic of SPI communication
			while ( SPI_SendDataIT(&SPI1handle,&dummy,1) == SPI_BUSY_TX); //The code runs till TX not busy
			while ( SPI_ReceiveDataIT(&SPI1handle,(uint8_t *)&ReadByte,1) == SPI_BUSY_RX ); //The code runs till RX not busy
		}


		if( ! (strcmp(RcvBuff,"nice" )) ){ //strcmp work like that if the message same, return 0
			GPIO_ToggleOutputPin(GPIOB, GPIO_Pin_2);
		}

		//Before disable the SPI, lets confirm SPI is not busy
		while(SPI_GetFlagStatus(SPI1, SPI_BUSY_FLAG));

		//Disable the SPI Peripheral
		SPI_PeriphControl(SPI1, DISABLE);


		//printf("Rcvd data = %s\n",RcvBuff);

		dataAvailable = 0;
		GPIO_IRQInterruptConfig(IRQn_EXTI0,ENABLE);

	}


}


// Runs when a data byte is received from the peripheral over SPI and fires to SPI_ApplicationEventCallback
void SPI1_IRQHandler()
{
	SPI_IRQHandling(&SPI1handle);
}



void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t AppEv)
{
	static uint32_t i = 0;
	// In the RX complete event , copy data in to RcvBuff . '\0' indicates end of message(rcvStop = 1)
	if(AppEv == SPI_EVENT_RX_CMPLT)
	{
				RcvBuff[i++] = ReadByte;
				if(ReadByte == '\0' || ( i == MAX_LEN)){ // Receive data until get null character from Ardiuno or when the data len reaches the max len
					rcvStop = 1; // Reception will stop here, end of the message
					RcvBuff[i-1] = '\0';// Last character in receive buffer will be null.
					i = 0;
				}

	}

}


/* Slave data available interrupt handler */
void EXTI0_IRQHandler()
{

	GPIO_IRQHandling(GPIO_Pin_0);
	dataAvailable = 1;
}
