/*
 * stm32f100xx_i2c.c
 *
 *  Created on: 12 Oca 2023
 *      Author: lutfullah_aydin
 */


/******************************************************************************/
/*                  ##### I2C Peripheral Module Driver #####	              */
/******************************************************************************/



#include "stm32f100xx_i2c.h"


static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_AddressWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_AddressRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle );
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle );



static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= ( 1 << I2C_CR1_START);
}


static void I2C_AddressWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr &= ~(1); //SlaveAddr is Slave address + r/nw bit=0
	pI2Cx->DR = SlaveAddr;
}


static void I2C_AddressRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr |= 1; //SlaveAddr is Slave address + r/nw bit=1
	pI2Cx->DR = SlaveAddr;
}

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle )
{
	uint32_t dummy_read;
	//check for device mode
	if(pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL)) //MSL: Slave mode:0, Master Mode:1
	{
		//Master mode
		if(pI2CHandle->TxRxState == I2C_BUSY_RX)
		{
			if(pI2CHandle->RxSize  == 1)
			{
				//first disable the ack
				I2C_ManageAcking(pI2CHandle->pI2Cx,DISABLE);

				//clear the ADDR flag ( read SR1 first then read SR2)
				dummy_read = pI2CHandle->pI2Cx->SR1;
				dummy_read = pI2CHandle->pI2Cx->SR2;
				(void)dummy_read; //Doing this to handle unused warning
			}

		}
		else
		{
			//clear the ADDR flag ( read SR1 first then read SR2)
			dummy_read = pI2CHandle->pI2Cx->SR1;
			dummy_read = pI2CHandle->pI2Cx->SR2;
			(void)dummy_read; // Doing this cause giving dummy error:))

		}

	}
	else
	{
		//If device is in Slave Mode
		//clear the ADDR flag ( read SR1 first then read SR2)
		dummy_read = pI2CHandle->pI2Cx->SR1;
		dummy_read = pI2CHandle->pI2Cx->SR2;
		(void)dummy_read;
	}


}


/*
 * If the applicaiton wants to release the bus, we can use this API
 * The bit is set and cleared by software, cleared by hardware when a Stop condition is
 * detected, set by hardware when a timeout error is detected.
 * In Master Mode:
 * 	   0: No Stop generation.
 * 	   1: Stop generation after the current byte transfer or after the current Start condition is sent.
 * In Slave mode:
 * 	   0: No Stop generation.
 * 	   1: Release the SCL and SDA lines after the current byte transfer.
 */
void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= ( 1 << I2C_CR1_STOP);
}


/*
 * Peripheral Clock Setup
 * I used only I2C1, So it is simple.
 */
void I2C_PCLK(I2C_RegDef_t *pI2Cx, uint8_t EnorDis)
{

	  if (EnorDis != DISABLE)
	  {
		  I2C1_PCLK_EN();
	  }else
	  {
		  I2C1_PCLK_DIS();
	  }
}




/*
 * I2C Peripheral Enable or Disable Configuration API
 */
void I2C_PeriphControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDis)
{
	  if (EnorDis != DISABLE)
	  {
		  pI2Cx->CR1 |= (1 << I2C_CR1_PE);

	  }else
	  {
		  pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
	  }
}



/*
 * Initializes the I2Cx with the pI2CHandle.
 *
 * 1. Configuring the Mode(Standart or Fast)
 * 2. Configuring the Speed of the Serial Clock(SCL) (Use CR2(FREQ) and CCR)
 * 3. Configuring the Device Address(When device is slave)
 * 4. Enable the Acking(Disable by default)
 * 5. Configure the rise time for I2C pin(Time required for signal from GND to Vcc)
 *
 * All the above Configurations must be done When I2C Peripheral Disable in CR1
 *
 * RCC_GetPCLK1Value() API implemented in rcc driver.
 */
void I2C_Init(I2C_Handle_t *pI2CHandle)
{

	//Enable the Clock first .  //I configured this setting inside the Init function because user could forget to enable the clock:)
	I2C_PCLK(pI2CHandle->pI2Cx, ENABLE);

	uint32_t tempreg;

	/**************************	I2C_CR1 Configuration *******************************/

	//Ack bit, Ack bit can be set only after I2C PE is 1.
	tempreg = 0;
	tempreg |= pI2CHandle->I2CConfig.I2C_ACK << I2C_CR1_ACK;
	pI2CHandle->pI2Cx->CR1 = tempreg;

	/**************************	I2C_CR2 Configuration *******************************/

	//FREQ Bit of Config
	tempreg = 0;
	tempreg |= RCC_GetPCLK1Value() / 1000000U; // 8 MHz diveded by , we only want 8 here.
	pI2CHandle->pI2Cx->CR2 = (tempreg & 0x3F); // Masking last 5 bit

	/**************************	I2C_OAR1 Configuration *******************************/

	//Store the device own address (When it is slave)
	tempreg = 0;
	tempreg |= pI2CHandle->I2CConfig.I2C_DeviceAddress << I2C_OAR1_ADD71;
	tempreg |= (1 << I2C_OAR1_BIT14); // In Reference Manual, 14th bit should always kept 1 by software.
	pI2CHandle->pI2Cx->OAR1 = tempreg;

	/**************************	I2C_CCR Configuration *******************************/

	uint16_t ccr_value = 0;
	tempreg = 0;
	if(pI2CHandle->I2CConfig.I2C_SCLSpeed <= I2C_SCL_SPEED_STD)//If it is Standard Mode
	{
		//Thigh = CCR * TPCLK1
		//Tlow = CCR * TPCLK1
		//This is the formula in Reference Man. More detail look at there
		//When Thigh = Tlow => Tscl = 2 * CCR * TPCLK1 => CCR is going to be like below.
		ccr_value = (RCC_GetPCLK1Value() / ( 2 * pI2CHandle->I2CConfig.I2C_SCLSpeed ) );
		tempreg |= (ccr_value & 0xFFF);// Masking last 12 bit by 0xFFF
	}else
	{
		//Fast Mode
		tempreg |= ( 1 << I2C_CCR_FS ); //Set the corresponding bit for fast mode
		tempreg |= (pI2CHandle->I2CConfig.I2C_FastModeDutyCycle << I2C_CCR_DUTY);
		if(pI2CHandle->I2CConfig.I2C_FastModeDutyCycle == I2C_Fast_Mode_DUTY_2) //When DUTY bit 0
		{
			ccr_value = (RCC_GetPCLK1Value() / ( 3 * pI2CHandle->I2CConfig.I2C_SCLSpeed ) );//Same derivation like STD mode above
		}else//When DUTY bit 1
		{
			ccr_value = (RCC_GetPCLK1Value() / ( 25 * pI2CHandle->I2CConfig.I2C_SCLSpeed ) );
		}
		tempreg |= (ccr_value & 0xFFF); // Masking last 12 bit by 0xFFF
	}
	pI2CHandle->pI2Cx->CCR = tempreg;


	/**************************	I2C_TRISE Configuration *******************************/

	if(pI2CHandle->I2CConfig.I2C_SCLSpeed <= I2C_SCL_SPEED_STD)
	{
		//mode is standard mode

		tempreg = (RCC_GetPCLK1Value() /1000000U) + 1 ;

	}else
	{
		//mode is fast mode
		tempreg = ( (RCC_GetPCLK1Value() * 300) / 1000000000U ) + 1;

	}

	pI2CHandle->pI2Cx->TRISE = (tempreg & 0x3F);


}



/*
 * De-Initializes the I2Cx with the pI2CHandle.
 * I used only I2C1, So it is simple.
 */
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
	I2C1_REG_RESET();  /* I2C1 clock reset */
}



/*
 * I2C Peripheral returns the Flag status in SR
 */
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
	if(pI2Cx->SR1 & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}



/*
 * Master Data Send with blocking(polling) Method
 * Look at the Master Transmit for I2C part of the Reference manual
 * "Sr" lets us to manage Repeated Start. This only makes sense when in multimaster modes
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle,uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr)
{
	// 1. Generate the START condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. confirm that start generation is completed by checking the Start Bit flag in the SR1
	//Until SB is cleared(Reading SR1 leads to clearing of SB) SCL will be stretched (pulled to LOW)
	while( !  I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_SB)   );

	//3. Send the address of the slave with r/nw bit set to w(0) (total 8 bits )
	I2C_AddressWrite(pI2CHandle->pI2Cx,SlaveAddr);

	//4. Confirm that address phase is completed by checking the ADDR flag in the SR1
	while( !  I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_ADDR)   );

	//5. clear the ADDR flag according to its software sequence(Look at the Reference Man)
	//   Note: Until ADDR is cleared SCL will be stretched (pulled to LOW)
	I2C_ClearADDRFlag(pI2CHandle);

	//6. send the data until len becomes 0
	while(Len > 0)
	{
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_TXE) ); //Wait till TXE is set
		pI2CHandle->pI2Cx->DR = *pTxbuffer;
		pTxbuffer++;
		Len--;
	}

	//7. when Len becomes zero we need to close communication, so wait for TXE=1 and BTF=1 before generating the STOP condition
	//   Note: TXE=1 , BTF=1 , means that both SR and DR are empty and next transmission should begin.
	//   Data comes to DR then goes to Shift register(SR). We need check "BTF=1" for to keep healty communication.
	//   when BTF=1 SCL will be stretched (pulled to LOW), So that it prevents to Underrun error in TX

	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_TXE) );
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_BTF) );


	//8. Generate STOP condition and master need not to wait for the completion of stop condition.
	//   Note: generating STOP, automatically clears the BTF
	if(Sr == I2C_DISABLE_SR ) // SR means Repeated Start
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

}


/*
 * Master Data Receive with blocking(polling) Method
 * Look at the Master Receive for I2C part of the Reference manual
 * "Sr" lets us to manage Repeated Start. This only makes sense when in multimaster modes
 */
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr,uint8_t Sr)
{

	//1. Generate the START condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. confirm that start generation is completed by checking the SB flag in the SR1
	//Until SB is cleared(Reading SR1 leads to clearing of SB) SCL will be stretched (pulled to LOW)
	while( !  I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_SB)   );

	//3. Send the address of the slave with r/nw bit set to R(1) (total 8 bits )
	I2C_AddressRead(pI2CHandle->pI2Cx,SlaveAddr);

	//4. wait until address phase is completed by checking the ADDR flag in teh SR1
	while( !  I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_ADDR)   );


	//procedure to read only 1 byte from slave
	if(Len == 1)
	{
		//Disable Acking before clear the ADDR(Because ADDR clear means the clock will be high)
		I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);


		//clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//wait until  RXNE becomes 1, do not read DR until it becomes
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_RXNE) );

		//generate STOP condition
		if(Sr == I2C_DISABLE_SR )// SR means Repeated Start
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		//read data in to buffer
		*pRxBuffer = pI2CHandle->pI2Cx->DR;

	}


    //procedure to read data from slave when Len > 1
	if(Len > 1)
	{
		//clear the ADDR flag, ADDR clear means the clock will be high,released
		I2C_ClearADDRFlag(pI2CHandle);

		//read the data until Len becomes zero
		for ( uint32_t i = Len ; i > 0 ; i--)
		{
			//wait until RXNE becomes 1, do not read DR until it becomes
			while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_RXNE) );

			if(i == 2) //if last 2 bytes are remaining
			{
				//Disable Acking
				I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);

				//generate STOP condition
				if(Sr == I2C_DISABLE_SR ) // SR means Repeated Start
					I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

			}

			//read the data from data register in to buffer
			*pRxBuffer = pI2CHandle->pI2Cx->DR;

			//increment the buffer address
			pRxBuffer++;

		}

	}

	//re-enable ACKing, cause we need to set acking set every times come to this API
	if(pI2CHandle->I2CConfig.I2C_ACK == I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_ENABLE);
	}

}



/*
 * Acking Disable by default, this function helps to manage acking
 */
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnorDis)
{
	if(EnorDis == I2C_ACK_ENABLE)
	{
		//enable the ack
		pI2Cx->CR1 |= ( 1 << I2C_CR1_ACK);
	}else
	{
		//disable the ack
		pI2Cx->CR1 &= ~( 1 << I2C_CR1_ACK);
	}
}


/*
 * NVIC register Configurations for Interrupts.
 * We have less than 96 IRQ numbers in this device header. First 3 register of below registers is enough for us.
 * IRQ(Interrupt Request) Number definitions are in stm32f1xx.h(device header file)
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDis)
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
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
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
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_TX) && (busystate != I2C_BUSY_RX))
	{
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		//This lead to set Start bit and it leads to an interrupt
		// Other phases of the communication handled by IRQHandling(Adress etc.)
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);

		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);

		//Data transmisson handled by I2C_EV_IRQHandling & I2C_ER_IRQHandling  !

	}

	return busystate;
}



/*
 * Data Receive with non-blocking(Interrupt) Method
 * This function returns the state
 */
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr,uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_TX) && (busystate != I2C_BUSY_RX))
	{
		pI2CHandle->pRxBuffer = pRxBuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_RX;
		pI2CHandle->RxSize = Len; // RxSize is used in ISR to manage data reception
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		//This lead to set Start bit and it leads to an interrupt
		// Other phases of the communication handled by IRQHandling(Adress etc.)
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);

		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);

		//Data reception handled by I2C_EV_IRQHandling & I2C_ER_IRQHandling  !

	}

	return busystate;
}




static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle )
{

	if(pI2CHandle->TxLen > 0)
	{
		//1. load the data in to DR
		pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

		//2. decrement the TxLen
		pI2CHandle->TxLen--;

		//3. Increment the buffer address
		pI2CHandle->pTxBuffer++;

	}

}

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle )
{
	//We have to do the data reception
	if(pI2CHandle->RxSize == 1)
	{
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->RxLen--;

	}


	if(pI2CHandle->RxSize > 1)
	{
		if(pI2CHandle->RxLen == 2)
		{
			//clear the ack bit
			I2C_ManageAcking(pI2CHandle->pI2Cx,DISABLE);
		}

			//read DR
			*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
			pI2CHandle->pRxBuffer++;
			pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxLen == 0 )
	{
		//close the I2C data reception and notify the application

		//1. generate the stop condition
		if(pI2CHandle->Sr == I2C_DISABLE_SR)
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		//2 . Close the I2C RX
		I2C_CloseReceiveData(pI2CHandle);

		//3. Notify the application
		I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_RX_CMPLT);
	}
}



/*
 * Reset all the member elements of the handle structure. Application state, len info etc.
 * This function helps to close interrupt based communication of receiving data.
 */
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
	//Implement the code to disable ITBUFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN);

	//Implement the code to disable ITEVFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxBuffer = NULL;
	pI2CHandle->RxLen = 0;
	pI2CHandle->RxSize = 0;

	if(pI2CHandle->I2CConfig.I2C_ACK == I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx,ENABLE);
	}

}

/*
 * Reset all the member elements of the handle structure. Application state, len info etc.
 * This function helps to close interrupt based communication of sending data.
 */
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	//Implement the code to disable ITBUFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN);

	//Implement the code to disable ITEVFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN);


	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->TxLen = 0;
}


/*
 * Slave side enable and disable Interrupt based callbacks.
 */
void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx,uint8_t EnorDis)
{
	 if(EnorDis == ENABLE)
	 {
			pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);
			pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);
			pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);
	 }else
	 {
			pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN);
			pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN);
			pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITERREN);
	 }

}

/*
 * Used for Slave to request for data from Master device in communication
 * Slave send the datas byte by byte, so only "data" argument is enough
 * Slave applications always be in interrupt mode
 */
void I2C_SlaveSendData(I2C_RegDef_t *pI2C,uint8_t data)
{
	pI2C->DR = data;
}

/*
 * Used for to Slave receive data from Master in communication
 * It returns the data in DR
 * Slave applications always be in interrupt mode
 */
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2C)
{
    return (uint8_t) pI2C->DR;
}




/*
 * I2C Event IRQ handling function for implement to non-blocking communication(Interrupt)
 * There are so many EVENTS that may trigger Interrrupts, It is considered as well inside the function
 * Interrupt handling issued for both master and slave mode(TXE & RXNE part) of a device
 * STOPF event generated at the end of Rx in Slave device
 */
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{

	uint32_t temp1, temp2, temp3;

	temp1   = pI2CHandle->pI2Cx->CR2 & ( 1 << I2C_CR2_ITEVTEN) ;
	temp2   = pI2CHandle->pI2Cx->CR2 & ( 1 << I2C_CR2_ITBUFEN) ;

	//Setting temp3 variable to SB flag check
	temp3  = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_SB);
/*****************************************************************************************/
	//1. When interrupt generated by SB(Start Bit) event
	//SB flag in SR1 is only applicable in Master mode. For slave mode SB is always zero though.
	if(temp1 && temp3)
	{
		//In this block, address phases(Read or Write) executed.
		if(pI2CHandle->TxRxState == I2C_BUSY_TX)
		{
			I2C_AddressWrite(pI2CHandle->pI2Cx,pI2CHandle->DevAddr);
		}else if (pI2CHandle->TxRxState == I2C_BUSY_RX )
		{
			I2C_AddressRead(pI2CHandle->pI2Cx,pI2CHandle->DevAddr);
		}
	}
/*****************************************************************************************/

/*****************************************************************************************/
	//2. When interrupt generated by ADDR event
	//Changing temp3 variable to ADDR flag check
	temp3  = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_ADDR);
	//   When master mode : Address is sent
	//	 When Slave mode  : Address matched with own address
	if(temp1 && temp3)
	{
		// interrupt is generated because of ADDR event
		I2C_ClearADDRFlag(pI2CHandle);
	}
/*****************************************************************************************/

/*****************************************************************************************/
	//3. When interrupt generated by BTF(Byte Transfer Finished) event
	//Changing temp3 variable to BTF flag check
	temp3  = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_BTF);
	if(temp1 && temp3)
	{
		//BTF flag is set
		if(pI2CHandle->TxRxState == I2C_BUSY_TX)
		{
			//make sure that TXE is also set .
			if(pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_TXE) )
			{
				//BTF, TXE = 1 ==> This is an indication of closing the communication.
			    //Both SR & DR is empty.
				//Close the communcation if only Txlen is 0
				if(pI2CHandle->TxLen == 0 )
				{
					//1. generate the STOP condition
					if(pI2CHandle->Sr == I2C_DISABLE_SR)
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

					//2. reset all the member elements of the handle structure. Application state, len info etc.
					I2C_CloseSendData(pI2CHandle);

					//3. notify the application about transmission complete
					I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_TX_CMPLT);

				}
			}

		}else if (pI2CHandle->TxRxState == I2C_BUSY_RX )
		{
			; // There is nothing to do.
		}
	}
/*****************************************************************************************/

/*****************************************************************************************/
	//4. When interrupt generated by STOPF event
	//Changing temp3 variable to STOPF flag check
	temp3  = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_STOPF);
	//Stop detection(STOPF) flag is applicable only slave mode . For master this flag will never be set
	if(temp1 && temp3)
	{
		//STOPF flag is set
		//We need to Clear the STOPF:
		// 1) Read SR1 == Already done by the code above
		// 2) Write to CR1
		pI2CHandle->pI2Cx->CR1 |= 0x0000; // Writing some unaffected data

		//Notify the application that STOPF is detected
		I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_STOPF);
	}
/*****************************************************************************************/

/*****************************************************************************************/
	//5. When interrupt generated by TXE event
	//Changing temp3 variable to TXE flag check
	temp3  = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_TXE);
	if(temp1 && temp2 && temp3)
	{
		//Check for device mode
		if(pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL))
		{
			//TXE flag is set, it is a indication of DR is empty.
			//We have to do the data transmission
			if(pI2CHandle->TxRxState == I2C_BUSY_TX)
			{
				I2C_MasterHandleTXEInterrupt(pI2CHandle); //Helper function
			}
		}else
		{
			//slave mode
			//make sure that the slave is really in transmitter mode
		    if(pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_TRA))
		    {
		    	I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_DATA_REQ);
		    }
		}
	}
/*****************************************************************************************/

/*****************************************************************************************/
	//6. When interrupt generated by RXNE event
	//Changing temp3 variable to RXNE flag check
	temp3  = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_RXNE);
	if(temp1 && temp2 && temp3)
	{
		//check device mode .
		if(pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL))
		{
			//The device is master

			//RXNE flag is set
			if(pI2CHandle->TxRxState == I2C_BUSY_RX)
			{
				I2C_MasterHandleRXNEInterrupt(pI2CHandle);

			}

		}else
		{
			//slave
			//make sure that the slave is really in receiver mode
			if(!(pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_TRA)))
			{
				I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_DATA_RCV);
			}
		}
	}
/*****************************************************************************************/

/*****************************************************************************************/
}




/*
 * I2C Error IRQ handling function for implement to non-blocking communication(Interrupt)
 * ACK Failure error generated for Slave end of Tx.
 */
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{

	uint32_t temp1,temp2;

    //Know the status of  ITERREN control bit in the CR2
	temp2 = (pI2CHandle->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN);


/***********************Check for Bus error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
	if(temp1  && temp2 )
	{
		//This is Bus error

		//Implement the code to clear the buss error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR);

		//Implement the code to notify the application about the error
	   I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_BERR);
	}

/***********************Check for arbitration lost error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
	if(temp1  && temp2)
	{
		//This is arbitration lost error

		//Implement the code to clear the arbitration lost error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_ARLO);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_ARLO);

	}

/***********************Check for ACK failure  error************************************/

	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//This is ACK failure error

	    //Implement the code to clear the ACK failure error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_AF);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_AF);
	}

/***********************Check for Overrun/underrun error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//This is Overrun/underrun

	    //Implement the code to clear the Overrun/underrun error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_OVR);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_OVR);
	}

/***********************Check for Time out error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//This is Time out error

	    //Implement the code to clear the Time out error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_TIMEOUT);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_TIMEOUT);
	}

}



/*
 * This function helps to manage program according to event happens in interrupt based communication
 */
__weak void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv)
{

	//This is a weak implementation . the user application may override this function.
}

