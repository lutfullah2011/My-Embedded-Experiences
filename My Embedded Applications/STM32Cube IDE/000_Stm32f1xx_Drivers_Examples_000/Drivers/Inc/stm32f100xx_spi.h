/*
 * stm32f100xx_spi.h
 *
 *  Created on: Dec 11, 2022
 *      Author: lutfullah_aydin
 */

#ifndef INC_STM32F100XX_SPI_H_
#define INC_STM32F100XX_SPI_H_


#include "stm32f1xx.h"


/******************************************************************************/
/*                  Structures for SPIx Peripheral			    	          */
/******************************************************************************/


/*
 * Configuration Structure for SPIx Peripheral
 */
typedef struct
{
  uint8_t SPI_DeviceMode;       		/* Specifies the SPI Modes whether Master or Slave(default)*/
  uint8_t SPI_BusConfig;       			/* Specifies the SPI Bus whether Full-dublex(default), Half-dublex or simplex(one way)*/
  uint8_t SPI_SclkSpeed;       			/* Specifies the SPI (Speed)Baud rate control*/
  uint8_t SPI_DFF;       				/* Specifies the SPI Data frame format whether 8 bit(default) or 16 bit */
  uint8_t SPI_CPOL;       				/* Specifies the SPI Clock polarity	*/
  uint8_t SPI_CPHA;       				/* Specifies the SPI Clock phase	*/
  uint8_t SPI_SPI_SSM;       			/* Specifies the SPI Software slave management whether software or hardware*/

}SPI_Config_t;



/*
 * Handle Structure for SPIx Peripheral
 */
typedef struct
{
	SPI_RegDef_t *pSPIx;				/*This holds the base address of SPIx Peripheral	*/
	SPI_Config_t SPIConfig;				/*This holds SPIx Peripheral configuration settings*/

}SPI_Handle_t;



/*
 * @SPI_DeviceMode
 */
#define SPI_DEVICE_MODE_MASTER		1
#define SPI_DEVICE_MODE_SLAVE		0

/*
 * @SPI_BusConfig
 */
#define SPI_BUS_CONFIG_FD				1 /*Full-Dublex*/
#define SPI_BUS_CONFIG_HD				2 /*Half-Dublex*/
#define SPI_BUS_CONFIG_S_RXONLY			3 /*Simplex-Receive only*/

/*
 * @SPI_SclkSpeed
 */
#define SPI_SCLK_SPEED_DIV2         	0
#define SPI_SCLK_SPEED_DIV4         	1
#define SPI_SCLK_SPEED_DIV8         	2
#define SPI_SCLK_SPEED_DIV16        	3
#define SPI_SCLK_SPEED_DIV32        	4
#define SPI_SCLK_SPEED_DIV64        	5
#define SPI_SCLK_SPEED_DIV128       	6
#define SPI_SCLK_SPEED_DIV256       	7

/*
 * @SPI_DFF
 */
#define SPI_DFF_8BITS		0
#define SPI_DFF_16BITS		1

/*
 * @SPI_CPOL
 */
#define SPI_CPOL_LOW		0
#define SPI_CPOL_HIGH		1

/*
 * @SPI_CPHA
 */
#define SPI_CPHA_LOW		0
#define SPI_CPHA_HIGH		1

/*
 * @SPI_SPI_SSM
 */
#define SPI_SSM_DIS		0
#define SPI_SSM_EN		1


/*
 * SPI Status Flag Definitions
 */
#define SPI_TXE_FLAG		(1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG		(1 << SPI_SR_RXNE)
#define SPI_BUSY_FLAG		(1 << SPI_SR_BSY)


/******************************************************************************/
/*                  Function Prototype by this driver - APIs	              */
/******************************************************************************/

/*
 * Peripheral Clock Setup
 */
void SPI_PCLK(SPI_RegDef_t *pSPIx, uint8_t EnorDis);/*Peripheral Clock Setup(Enable or Disable)*/

/*
 * SPIx Peripheral Init and De-Init
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);	/*Peripheral Initialization*/
void SPI_DeInit(SPI_RegDef_t *pSPIx);		/*Peripheral Deinitialization*/

/*
 * Data Send and Receive
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);

/*
 * IRQ Confiiguration and ISR Handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDis);/*IRQ Interrupt Configuration(Enable or Disable)*/
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority); /*IRQ Priority Configuration*/
void SPI_IRQHandling(SPI_Handle_t *pHandle); /*ISR(Interrupt Service Routine) Handling*/


/*
 * Other SPI Peripheral Control API
 */
void SPI_PeriphControl(SPI_RegDef_t *pSPIx, uint8_t EnorDis);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDis);
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDis);
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName);

#endif /* INC_STM32F100XX_SPI_H_ */
