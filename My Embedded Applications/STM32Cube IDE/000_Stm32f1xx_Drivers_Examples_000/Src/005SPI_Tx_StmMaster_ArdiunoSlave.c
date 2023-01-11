/*
 * 005SPI_Tx_StmMaster_ArdiunoSlave.c
 *
 *  Created on: 21 Ara 2022
 *      Author: lutfullah_aydin
 */


#include "stm32f1xx.h"
#include <string.h>



//char testMessage[]= "Lütfullah AYDINN";
//char testMessage[]= "The serial peripheral interface (SPI) allows half/ full-duplex, synchronous, serial communication with external devices. The interface can be configured as the master and in this case it provides the communication clock (SCK) to the external slave device.";


void gpioConfig(){

	GPIO_Handle_t  pGPIOspi, GPIOLed, GPIOButton;

	//For SPI1 TX
	pGPIOspi.pGPIOx = GPIOA;
	pGPIOspi.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_AF_PP;
	pGPIOspi.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_MEDIUM;

	//For SCLK
	pGPIOspi.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(&pGPIOspi);

	//For MOSI
	pGPIOspi.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(&pGPIOspi);

	//For NCC
	pGPIOspi.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_OUT_PP;
	// Bu kismi yaoinca pin low olmuyor anlamadim, SSOE biti 1 olunca NSS pini neden low olmuyor?//pGPIOspi.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_INPUT_PU;
	pGPIOspi.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(&pGPIOspi);


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
 * PA4 --> SPI1_NCC - General Output
 * PA5 --> SPI1_SCK - Alternate Output PP
 * PA6 --> SPI1_MISO - INPUT FLOATING
 * PA7 --> SPI1_MOSI - Alternate Output PP
 */
void spiConfig(){

	SPI_Handle_t SPIHandle;

	SPIHandle.pSPIx = SPI1;
	SPIHandle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPIHandle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPIHandle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPIHandle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV32;// fclock generates 8Mhz and  sclk is now 2mhz.
	SPIHandle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPIHandle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPIHandle.SPIConfig.SPI_SPI_SSM = SPI_SSM_DIS; //Software slave managment disabled, we will use hardware slave managment


	SPI_Init(&SPIHandle);
}

void delay(uint32_t time){

	while(time--);

}

int main(void)
{

	char testMessage[]= "Lutfullah";


	gpioConfig(); // GPIO INIT
	spiConfig();  // SPI INIT

	/*
	 * SSOE = 1 means NSS output enable.
	 * NSS pin is managed by hardware.
	 * When spi enabled, NSS will pulled to low.
	 * When spi disabled, NSS will pulled to high.
	 */

	SPI_SSOEConfig(SPI1, ENABLE); // Setting SSOE pin, NSS pin will pull to low in order to activate slave device(Ardiuno in this scenario)


	while (1)
	{
		delay(500000);
		GPIO_ToggleOutputPin(GPIOB, GPIO_Pin_1);

		while ( ! (GPIO_ReadFromInputPin(GPIOA, GPIO_Pin_0)) ); // Data will send only if I press the button. Because code will not hanging in "here (while)" when I press the button

		delay(500000); //small delay for button debouncing issues.

		GPIO_ToggleOutputPin(GPIOB, GPIO_Pin_2);

		//Enable the SPI Peripheral
		SPI_PeriphControl(SPI1, ENABLE);

		//First, sending length of the data to Slave device(Ardiuno) in order to know the size of data.
		uint8_t datalen = strlen(testMessage);
		SPI_SendData(SPI1, &datalen, 1 );

		//Then, sending data
		SPI_SendData(SPI1, (uint8_t*) testMessage, strlen(testMessage));


		//Before disable the SPI, lets confirm SPI is not busy
		while(SPI_GetFlagStatus(SPI1, SPI_BUSY_FLAG));

		//Disable the SPI Peripheral
		SPI_PeriphControl(SPI1, DISABLE);

	}


}


