/*
 * stm32f100xx_spi.c
 *
 *  Created on: Dec 11, 2022
 *      Author: lutfullah_aydin
 */

/******************************************************************************/
/*                  ##### SPI Peripheral Module Driver #####	              */
/******************************************************************************/


#include "stm32f100xx_spi.h"


/*
 * Peripheral Clock Setup
 * I used only SPI1, So it is simple.
 */
void SPI_PCLK(SPI_RegDef_t *pSPIx, uint8_t EnorDis)
{

	  if (EnorDis != DISABLE)
	  {
		  SPI1_PCLK_EN();
	  }else
	  {
		  SPI1_PCLK_DIS();
	  }
}



/*
 * Initializes the SPIx with the pSPIHandle.
 */
void SPI_Init(SPI_Handle_t *pSPIHandle)
{

	//Enable the Clock first .  //I configured this setting inside the Init function because user could forget to enable the clock:)
	SPI_PCLK(pSPIHandle->pSPIx, ENABLE);


	/**************************	SPI_CR1 Configuration *******************************/
	uint32_t tempreg = 0;

	//1. Device mode configuration
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR ;


	//2. Bus configuration(Direction)
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD) //Full dublex
	{
		tempreg &= ~(1<<SPI_CR1_BIDIMODE); //Clear BIDIMODE bit in CR1 Reg

	}else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD) //Half dublex
	{
		tempreg |= (1<<SPI_CR1_BIDIMODE); //Set BIDIMODE bit in CR1 Reg

	}else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_S_RXONLY) //Simplex Receive Only
	{
		tempreg &= ~(1<<SPI_CR1_BIDIMODE); //Clear BIDIMODE bit in CR1 Reg
		tempreg |= (1<<SPI_CR1_RXONLY); //Set RXONLY bit in CR1 Reg (output disabled)
	}

	//3. (Speed)Baud rate control Configuration
	tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

	//4. DFF(Data Frame Format) Configuration
	tempreg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF ;

	//5. CPOL Configuration
	tempreg |= pSPIHandle->SPIConfig.SPI_CPOL <<SPI_CR1_CPOL;

	//6. CPHA Configuration
	tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

	//7. SSM Configuration
	tempreg |= pSPIHandle->SPIConfig.SPI_SPI_SSM << SPI_CR1_SSM;


	pSPIHandle->pSPIx->CR1 = tempreg; //putting all settings into actual place which is CR1 reg.
}



/*
 * De-Initializes the SPIx with the pSPIHandle.
 * I used only SPI1, So it is simple.
 */
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	SPI1_REG_RESET();  /* SPI1 clock reset */

//	if (pSPIx == SPI1)
//	{
//		SPI1_REG_RESET();  /* SPI1 clock reset */
//
//	}else if (pSPIx == SPI2)
//	{
//		SPI2_REG_RESET();  /* SPI2 clock reset */
//
//	}else if (pSPIx == SPI3)
//	{
//		SPI3_REG_RESET();  /* SPI3 clock reset */
//	}
}


uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if(pSPIx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}


/*
 * Data Send with blocking(polling) Method
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	while(Len > 0)//until no data remain
	{
		while (SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET); //Check transmit buffer empty or not! Wait until TXE is set!!

		if(pSPIx->CR1 & (1 << SPI_CR1_DFF)) // Check data is 8 bit or 16 bit format!
		{
			pSPIx->DR = *((uint16_t*)pTxBuffer); //16 bit data loads to the DR
			Len--;
			Len--; // 2 times decrease because we just send 2 bytes of data
			(uint16_t*)pTxBuffer++;
		}else
		{
			pSPIx->DR = *pTxBuffer; //8 bit data loads to the DR
			Len--; // 1 times decrease because we just send 1 bytes of data
			pTxBuffer++; // data buffer increased for next byte of data

		}
	}
}



/*
 * Data Receive with blocking(polling) Method
 */
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
	while(Len > 0)//until no data remain
	{
		while (SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG)  == (uint8_t)FLAG_RESET ); //Check Receive buffer not empty ! Wait until RXNE is set!!

		if(pSPIx->CR1 & (1 << SPI_CR1_DFF)) // Check data is 8 bit or 16 bit format!
		{
			*((uint16_t*)pRxBuffer) = pSPIx->DR; //16 bit data reads from the DR
			Len--;
			Len--; // 2 times decrease because we just receive 2 bytes of data
			(uint16_t*)pRxBuffer++;
		}else
		{
			*pRxBuffer = pSPIx->DR; //8 bit data reads from the DR
			Len--; // 1 times decrease because we just receive 1 bytes of data
			pRxBuffer++; // data buffer increased for next byte of data

		}
	}
}




/*
 * SPI Peripheral Enable or Disable Configuration API
 */
void SPI_PeriphControl(SPI_RegDef_t *pSPIx, uint8_t EnorDis)
{
	  if (EnorDis != DISABLE)
	  {
		  pSPIx->CR1 |= (1 << SPI_CR1_SPE);

	  }else
	  {
		  pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	  }
}


/*
 * SPI SSI bit Enable or Disable Configuration API
 */
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDis)
{
	  if (EnorDis != DISABLE)
	  {
		  pSPIx->CR1 |= (1 << SPI_CR1_SSI);

	  }else
	  {
		  pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	  }
}


/*
 * SPI SSOE bit Enable or Disable Configuration API
 */
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDis)
{
	  if (EnorDis != DISABLE)
	  {
		  pSPIx->CR2 |= (1 << SPI_CR2_SSOE);

	  }else
	  {
		  pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	  }
}









/*
 * Data Send with non-blocking(Interrupt) Method
 */
/*
 * Data Receive with non-blocking(Interrupt) Method
 */



/*
 * NVIC register Configurations for Interrupts.
 * We have less than 96 IRQ numbers in this device header. First 3 register of below registers is enough for us.
 * IRQ(Interrupt Request) Number definitions are in stm32f1xx.h(device header file)
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDis)
{
}



/*
 *IRQ Priority Configuration
 */
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
}



/*
 * IRQ Handling function will use inside the actual IRQ_Handling
 * function implemented as weak in startup file.
 */
void SPI_IRQHandling(SPI_Handle_t *pHandle)
{
}






//
//
//if(pSPIHandle->SPIConfig.SPI_SclkSpeed == SPI_SCLK_SPEED_DIV2) // fPCLK/2
//{
//	tempreg &= ~(7<<3); //Clear BR[2:0] in CR1 Reg for default(000)
//
//}else if(pSPIHandle->SPIConfig.SPI_SclkSpeed == SPI_SCLK_SPEED_DIV4)
//{
//	tempreg |= (1<<3);
//
//}else if(pSPIHandle->SPIConfig.SPI_SclkSpeed == SPI_SCLK_SPEED_DIV8)
//{
//	tempreg |= (2<<3);
//
//}else if(pSPIHandle->SPIConfig.SPI_SclkSpeed == SPI_SCLK_SPEED_DIV16)
//{
//	tempreg |= (3<<3);
//
//}else if(pSPIHandle->SPIConfig.SPI_SclkSpeed == SPI_SCLK_SPEED_DIV32)
//{
//	tempreg |= (4<<3);
//
//}else if(pSPIHandle->SPIConfig.SPI_SclkSpeed == SPI_SCLK_SPEED_DIV64)
//{
//	tempreg |= (5<<3);
//
//}else if(pSPIHandle->SPIConfig.SPI_SclkSpeed == SPI_SCLK_SPEED_DIV128)
//{
//	tempreg |= (6<<3);
//
//}else if(pSPIHandle->SPIConfig.SPI_SclkSpeed == SPI_SCLK_SPEED_DIV256)
//{
//	tempreg |= (7<<3);
//}


