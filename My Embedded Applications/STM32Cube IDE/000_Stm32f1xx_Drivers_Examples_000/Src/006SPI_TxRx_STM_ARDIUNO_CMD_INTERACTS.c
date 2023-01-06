/*
 * 006SPI_TxRx_STM_ARDIUNO_CMD_INTERACTS.c
 *
 *  Created on: 4 Oca 2023
 *      Author: lutfullah_aydin
 */



#include "stm32f1xx.h"
#include  <string.h>
#include  <stdio.h>


//Command Codes to interface with ARDIUNO UNO
#define COMMAND_LED_CTRL      		0x50
#define COMMAND_SENSOR_READ      	0x51
#define COMMAND_LED_READ      		0x52
#define COMMAND_PRINT      			0x53
#define COMMAND_ID_READ      		0x54


#define LED_ON     1
#define LED_OFF    0

//Ardiuno Analog pins
#define ANALOG_PIN0 	0
#define ANALOG_PIN1 	1
#define ANALOG_PIN2 	2
#define ANALOG_PIN3 	3
#define ANALOG_PIN4 	4

//Arduino led
#define LED_PIN  9


//FUNCTION DECLERATION
void command_led_control();
void command_read_sensor();
void command_read_led();
void command_what_yo_want_to_print();
void command_check_id();
uint8_t SPI_ResponseCheck(uint8_t ackbyte);

//PRIVATE VARIABLE DEFINITIONS
uint8_t empty_write = 0xFF;
uint8_t empty_read;
uint8_t commandcode;
uint8_t ackbyte;
uint8_t args[2];


void gpioConfig(){

	GPIO_Handle_t  pGPIOspi, GPIOLed, GPIOButton;

	//For SPI1

	/*
	 * PA4 --> SPI1_NCC - General Output
	 * PA5 --> SPI1_SCK - Alternate Output PP
	 * PA6 --> SPI1_MISO - INPUT FLOATING
	 * PA7 --> SPI1_MOSI - Alternate Output PP
	 */
	pGPIOspi.pGPIOx = GPIOA;
	pGPIOspi.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_AF_PP;
	pGPIOspi.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_MEDIUM;

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
	GPIOLed.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_1;
	GPIOLed.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_MEDIUM;
	GPIO_Init(&GPIOLed);

	//For button
	GPIOButton.pGPIOx = GPIOA;
	GPIOButton.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_INPUT_PD;
	GPIOButton.GPIO_PinConfig.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(&GPIOButton);

}



void spiConfig(){

	SPI_Handle_t SPIHandle;

	SPIHandle.pSPIx = SPI1;
	SPIHandle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPIHandle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPIHandle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPIHandle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV16;// fclock generates 8Mhz and  sclk is now mhz.
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

		GPIO_ToggleOutputPin(GPIOB, GPIO_Pin_1);

		//Enable the SPI Peripheral
		SPI_PeriphControl(SPI1, ENABLE);

		while ( ! (GPIO_ReadFromInputPin(GPIOA, GPIO_Pin_0)) ); // Data will send only if I press the button. Because code will not hanging in "here (while)" when I press the button
		GPIO_ToggleOutputPin(GPIOB, GPIO_Pin_1);
		delay(500000); //small delay for button debouncing issues.
		command_led_control();

		while ( ! (GPIO_ReadFromInputPin(GPIOA, GPIO_Pin_0)) );
		GPIO_ToggleOutputPin(GPIOB, GPIO_Pin_1);
		delay(500000); //small delay for button debouncing issues.
		command_read_sensor();

		while ( ! (GPIO_ReadFromInputPin(GPIOA, GPIO_Pin_0)) );
		GPIO_ToggleOutputPin(GPIOB, GPIO_Pin_1);
		delay(500000); //small delay for button debouncing issues.
		command_read_led();

		while ( ! (GPIO_ReadFromInputPin(GPIOA, GPIO_Pin_0)) );
		GPIO_ToggleOutputPin(GPIOB, GPIO_Pin_1);
		delay(500000); //small delay for button debouncing issues.
		command_what_yo_want_to_print();

		while ( ! (GPIO_ReadFromInputPin(GPIOA, GPIO_Pin_0)) );
		GPIO_ToggleOutputPin(GPIOB, GPIO_Pin_1);
		delay(500000); //small delay for button debouncing issues.
		command_check_id();


		//Before disable the SPI, lets confirm SPI is not busy
		while(SPI_GetFlagStatus(SPI1, SPI_BUSY_FLAG));

		//Disable the SPI Peripheral
		SPI_PeriphControl(SPI1, DISABLE);

	}


}




uint8_t SPI_ResponseCheck(uint8_t ackbyte)
{

	if(ackbyte == (uint8_t)0xF5)
	{
		return 1; //ACK ise 1 değilse 0 döndür!
	}

	return 0;
}



void command_led_control()
{
    //1. CMD_LED_CTRL takes pin no(1) &  value(1)

	commandcode = COMMAND_LED_CTRL;

	//send command
	SPI_SendData(SPI1,&commandcode,1);

	//do dummy read to clear off the RXNE
	SPI_ReceiveData(SPI1,&empty_read,1);

	// IMPORTANT NOTE : When Master or Slave sends 1 byte, it also receive 1 byte in return. Thats why I am doing dummy reads & writes

	//Send some dummy bits (1 byte) fetch the response from the slave
	SPI_SendData(SPI1,&empty_write,1);

	//read the ack byte received
	SPI_ReceiveData(SPI1,&ackbyte,1);

	if( SPI_ResponseCheck(ackbyte))
	{
		args[0] = LED_PIN;
		args[1] = LED_ON;

		//send arguments
		SPI_SendData(SPI1,args,2);
		// dummy read
		SPI_ReceiveData(SPI1,args,2);
	}

}


void command_read_sensor()
 {
	//2. CMD_SENSOR_READ   needs analog pin number(1)

	commandcode = COMMAND_SENSOR_READ;

	//send command
	SPI_SendData(SPI1, &commandcode, 1);

	//do dummy read to clear off the RXNE
	SPI_ReceiveData(SPI1, &empty_read, 1);

	//Send some dummy byte to fetch the response from the slave
	SPI_SendData(SPI1, &empty_write, 1);

	//read the ack byte received
	SPI_ReceiveData(SPI1, &ackbyte, 1);

	if (SPI_ResponseCheck(ackbyte)) {
		args[0] = ANALOG_PIN0;

		//send arguments
		SPI_SendData(SPI1, args, 1); //sending one byte of

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI1, &empty_read, 1);

		//insert some delay so that slave can ready with the data
		delay(50000);

		//Send some dummy bits (1 byte) fetch the response from the slave
		SPI_SendData(SPI1, &empty_write, 1);

		uint8_t analog_read;
		SPI_ReceiveData(SPI1, &analog_read, 1);
	}
}

void command_read_led()
{
	//3.  CMD_LED_READ 	needs pin no(1)
	commandcode = COMMAND_LED_READ;

	//send command
	SPI_SendData(SPI1,&commandcode,1);

	//do dummy read to clear off the RXNE
	SPI_ReceiveData(SPI1,&empty_read,1);

	//Send some dummy byte to fetch the response from the slave
	SPI_SendData(SPI1,&empty_write,1);

	//read the ack byte received
	SPI_ReceiveData(SPI1,&ackbyte,1);

	if( SPI_ResponseCheck(ackbyte))
	{
		args[0] = LED_PIN;

		//send arguments
		SPI_SendData(SPI1,args,1); //sending one byte of

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI1,&empty_read,1);

		//insert some delay so that slave can ready with the data
		delay(50000);

		//Send some dummy bits (1 byte) fetch the response from the slave
		SPI_SendData(SPI1,&empty_write,1);

		uint8_t led_status;
		SPI_ReceiveData(SPI1,&led_status,1);

	}
}

void command_what_yo_want_to_print()
{
	//4. CMD_PRINT needs len(2) &  message(len)
	commandcode = COMMAND_PRINT;

	//send command
	SPI_SendData(SPI1,&commandcode,1);

	//do dummy read to clear off the RXNE
	SPI_ReceiveData(SPI1,&empty_read,1);

	//Send some dummy byte to fetch the response from the slave
	SPI_SendData(SPI1,&empty_write,1);

	//read the ack byte received
	SPI_ReceiveData(SPI1,&ackbyte,1);

	uint8_t message[] = "Hello ! It's Lutfullah ??";
	if( SPI_ResponseCheck(ackbyte))
	{
		args[0] = strlen((char*)message);

		//send arguments
		SPI_SendData(SPI1,args,1); //sending length

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI1,&empty_read,1);

		delay(50000);

		//send message
		for(int i = 0 ; i < args[0] ; i++)
		{
			SPI_SendData(SPI1,&message[i],1);
			SPI_ReceiveData(SPI1,&empty_read,1);
		}

	}
}

void command_check_id()
{
	//5. CMD_ID_READ
	commandcode = COMMAND_ID_READ;

	//send command
	SPI_SendData(SPI1,&commandcode,1);

	//do dummy read to clear off the RXNE
	SPI_ReceiveData(SPI1,&empty_read,1);

	//Send some dummy byte to fetch the response from the slave
	SPI_SendData(SPI1,&empty_write,1);

	//read the ack byte received
	SPI_ReceiveData(SPI1,&ackbyte,1);

	uint8_t id[11];
	uint32_t i=0;
	if( SPI_ResponseCheck(ackbyte))
	{
		//read 10 bytes id from the slave
		for(  i = 0 ; i < 10 ; i++)
		{
			//send dummy byte to fetch data from slave
			SPI_SendData(SPI1,&empty_write,1);
			SPI_ReceiveData(SPI1,&id[i],1);
		}

		id[10] = '\0';

	}
}
