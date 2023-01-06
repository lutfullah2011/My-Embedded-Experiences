/*
 * 004SPI_TX.c
 *
 *  Created on: 16 Ara 2022
 *      Author: lutfullah_aydin
 */


#include "stm32f1xx.h"
#include <string.h>


char testMessage[]= "Lütfullah AYDIN";


void gpioConfig(){

	GPIO_Handle_t  pGPIOspi;

	//For SPI1 TX
	pGPIOspi.pGPIOx = GPIOA;
	pGPIOspi.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_AF_PP;
	pGPIOspi.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7;// In this application, we dont need to use MISO and NSS pin but lets keep it for now
	pGPIOspi.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_MEDIUM;

	GPIO_Init(&pGPIOspi);

}


/*
 * PA4 --> SPI1_NCC
 * PA5 --> SPI1_SCK
 * PA6 --> SPI1_MISO
 * PA7 --> SPI1_MOSI
 */
void spiConfig(){

	SPI_Handle_t SPIHandle;

	SPIHandle.pSPIx = SPI1;
	SPIHandle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPIHandle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPIHandle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPIHandle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2;// fclock generates 8Mhz and  sclk is now 4mhz.
	SPIHandle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPIHandle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPIHandle.SPIConfig.SPI_SPI_SSM = SPI_SSM_EN; //Software slave managment enabled for NSS pin but this creates a fault
												  //and make MSTR pin to reset(slave mode) which we do not want. So we need
												  //to make SSI=1 to tie NSS to +VCC internaly. SSI is 0 by default and it leads multi-master situation.


	SPI_Init(&SPIHandle);
}

void delay(uint32_t time){

	while(time--);

}

int main(void)
{
	gpioConfig();
	spiConfig();

	//This makes NSS signal internally high. Means we are not using multi master. For more detail, go to reference manual CR1 reg.
	SPI_SSIConfig(SPI1, ENABLE);


	/*
	SPE bit in SPI_CR1 Register is 0 by default = means SPI is diabled. We need to enable it
	When a transfer is terminated, the application can stop the communication by disabling the
	SPI peripheral. This is done by clearing the SPE bit.
	For some configurations, disabling the SPI and entering the Halt mode while a transfer is
	ongoing can cause the current transfer to be corrupted and/or the BSY flag might become
	unreliable.
	*/
	//Enable the SPI Peripheral
	SPI_PeriphControl(SPI1, ENABLE);

	SPI_SendData(SPI1, (uint8_t *)testMessage, strlen(testMessage));

	//Before disable the SPI, lets confirm SPI is not busy
	while(SPI_GetFlagStatus(SPI1, SPI_BUSY_FLAG));

	//Disable the SPI Peripheral
	SPI_PeriphControl(SPI1, DISABLE);

	while(1);



	return 0;
}
