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


static void  spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void  spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void  spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);



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


/*
 * SPI Peripheral returns the Flag status in SR
 */
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
 * NVIC register Configurations for Interrupts.
 * We have less than 96 IRQ numbers in this device header. First 3 register of below registers is enough for us.
 * IRQ(Interrupt Request) Number definitions are in stm32f1xx.h(device header file)
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDis)
{

	if(EnorDis == ENABLE)
	{
		if(IRQNumber <= 31) // ISER0 register (Interrupt Set-Enable Register)
		{
			*NVIC_ISER0 |= (1 << IRQNumber);

		}else if(IRQNumber > 31 && IRQNumber < 64) // ISER1 register (Interrupt Set-Enable Register)
		{
			*NVIC_ISER1 |= (1 << IRQNumber % 32);

		}else if(IRQNumber > 64 && IRQNumber < 96) // ISER2 register (Interrupt Set-Enable Register)
		{
			*NVIC_ISER2 |= (1 << IRQNumber % 64);
		}

	}else
	{
		if(IRQNumber <= 31)// ICER0 register (Interrupt Clear-Enable Register)
		{
			*NVIC_ICER0 |= (1 << IRQNumber);

		}else if(IRQNumber > 31 && IRQNumber < 64)// ICER1 register (Interrupt Clear-Enable Register)
		{
			*NVIC_ICER1 |= (1 << IRQNumber % 32);

		}else if(IRQNumber > 64 && IRQNumber < 96)// ICER2 register (Interrupt Clear-Enable Register)
		{
			*NVIC_ICER2 |= (1 << IRQNumber % 64);
		}

	}

}



/*
 *IRQ Priority Configuration
 */
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// Find out the IPR(Interrupt Priority Register)
	uint8_t iprx = IRQNumber / 4; //each iprx is 8 bit, 32 bit is divided by 4
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8- NO_PR_BITS_IMPLEMENTED); //PR=Priority & NO_PR_BITS_IMPLEMENTED=4 // Every 8 bits of iprx's first 4 bit is not used. So, we just left shifted 4 times. Thats it

	*(NVIC_PR_BASE_ADDR +  iprx ) |= ( IRQPriority << shift_amount ) ;
}







/*
 * Data Send with non-blocking(Interrupt) Method
 * This function returns the state
 */
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;

	if(state != SPI_BUSY_TX)
	{
		// Save the Tx buffer address and Len information in some SPI_Handle_t struct.
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;

		//SPI is in the busy state with transmission business, no takes over
		pSPIHandle->TxState = SPI_BUSY_TX;

		//TXEIE bit Enabled to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= ( 1 << SPI_CR2_TXEIE );

		//Data transmission handled by IRQHandling !

	}


	return state;
}




/*
 * Data Receive with non-blocking(Interrupt) Method
 * This function returns the state
 */
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->RxState;

	if(state != SPI_BUSY_RX)
	{
		// Save the Rx buffer address and Len information in some SPI_Handle_t struct.
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;

		//SPI is in the busy state with reception business, no takes over
		pSPIHandle->RxState = SPI_BUSY_RX;

		//RXNEIE bit Enabled to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= ( 1 << SPI_CR2_RXNEIE );

		//Data reception handled by IRQHandling !


	}


	return state;

}






/*
 * IRQ Handling function will use inside the actual IRQ_Handling
 * function implemented as weak in startup file.
 */
void SPI_IRQHandling(SPI_Handle_t *pHandle)
{

	uint8_t temp1 , temp2;
	//Checking TXE
	temp1 = pHandle->pSPIx->SR & ( 1 << SPI_SR_TXE); //Test the TXE flag set or not in the SR
	temp2 = pHandle->pSPIx->CR2 & ( 1 << SPI_CR2_TXEIE);//Test the TXEIE set or not in the CR2

	if( temp1 && temp2) // if txe & txeie are set
	{
		//handle TXE
		spi_txe_interrupt_handle(pHandle);
	}

	//Checking RXNE
	temp1 = pHandle->pSPIx->SR & ( 1 << SPI_SR_RXNE); //Test the RXNE flag set or not in the SR
	temp2 = pHandle->pSPIx->CR2 & ( 1 << SPI_CR2_RXNEIE); //Test the RXNEIE set or not in the CR2

	if( temp1 && temp2) // if rxne & rxneie are set
	{
		//handle RXNE
		spi_rxne_interrupt_handle(pHandle);
	}

	// Checking OVR Flag
	temp1 = pHandle->pSPIx->SR & ( 1 << SPI_SR_OVR);//Test the OVR flag set or not in the SR
	temp2 = pHandle->pSPIx->CR2 & ( 1 << SPI_CR2_ERRIE); //Test the ERRIE set or not in the CR2

	if( temp1 && temp2) // if ovr & errie are set
	{
		//handle ovr error
		spi_ovr_err_interrupt_handle(pHandle);
	}


}



//These Functions created for helping the IRQ Handling. All is static, not allowed to outside.

static void  spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	if( (pSPIHandle->pSPIx->CR1 & ( 1 << SPI_CR1_DFF) ) )// Check data is 8 bit or 16 bit format!
	{
		pSPIHandle->pSPIx->DR =   *((uint16_t*)pSPIHandle->pTxBuffer); //16 bit data loads to the DR
		pSPIHandle->TxLen--;
		pSPIHandle->TxLen--; // 2 times decrease because we just send 2 bytes of data
		(uint16_t*)pSPIHandle->pTxBuffer++;
	}else
	{
		//8 bit DFF
		pSPIHandle->pSPIx->DR =  *pSPIHandle->pTxBuffer; //8 bit data loads to the DR
		pSPIHandle->TxLen--; // 1 times decrease because we just send 1 bytes of data
		pSPIHandle->pTxBuffer++;  // data buffer increased for next byte of data
	}


	if(! pSPIHandle->TxLen) //If TxLen is zero , close the transmission
	{

		//this prevents interrupts from setting up of TXE flag
		SPI_CloseTransmisson(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_TX_CMPLT); // Inform applicaiton TX is done
	}

}


static void  spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	if(pSPIHandle->pSPIx->CR1 & ( 1 << SPI_CR1_DFF)) // Check data is 8 bit or 16 bit format!
	{
		//16 bit data reads from the DR
		*((uint16_t*)pSPIHandle->pRxBuffer) = (uint16_t) pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen -= 2; // 2 times decrease because we just receive 2 bytes of data
		pSPIHandle->pRxBuffer++;
		pSPIHandle->pRxBuffer++;// 2 times increase

	}else
	{
		//8 bit data reads from the DR
		*(pSPIHandle->pRxBuffer) = (uint8_t) pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--; // 1 times decrease because we just receive 1 bytes of data
		pSPIHandle->pRxBuffer++; // data buffer increased for next byte of data
	}


	if(! pSPIHandle->RxLen) //If RxLen is zero , close the reception
	{
		//Reception is complete
		SPI_CloseReception(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_RX_CMPLT); // Inform applicaiton RX is done
	}

}


static void  spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{

	uint8_t temp;
	//Clear the OVR flag
	if(pSPIHandle->TxState != SPI_BUSY_TX)
	{
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
	}
	(void)temp; // program gives error that temp variable is not used. So I used like this
	//Inform the application overrun error.
	//When application receive this event, application has to call the SPI_ClearOVRFlag
	SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_OVR_ERR);

}


void SPI_CloseTransmisson(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~( 1 << SPI_CR2_TXEIE); // Clear the TX interrupt bit
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;

}

void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~( 1 << SPI_CR2_RXNEIE); // Clear the RX interrupt bit
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;

}



void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx) // Reading DR & SR clears the OVR flag
{
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void)temp; // program gives error that temp variable is not used. So I used like this

}



__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t AppEv)
{

	//This is a weak implementation . the user application may override this function.
}



