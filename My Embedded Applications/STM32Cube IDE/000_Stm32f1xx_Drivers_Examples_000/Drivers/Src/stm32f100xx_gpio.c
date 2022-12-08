/*
 * stm32f100xx_gpio.c
 *
 *  Created on: 15 Kas 2022
 *      Author: lutfullah_aydin
 */

/******************************************************************************/
/*                  ##### GPIO Peripheral Module Driver #####	              */
/******************************************************************************/


#include "stm32f100xx_gpio.h"


/*Peripheral Clock Setup(Enable or Disable)*/
void GPIO_PCLK(GPIO_RegDef_t *pGPIOx, uint8_t EnorDis)
{
	  if (EnorDis != DISABLE)
	  {
	      if(pGPIOx == GPIOA)
	      {
	    	  GPIOA_PCLK_EN();

	      }else if(pGPIOx == GPIOB)
	      {
	    	  GPIOB_PCLK_EN();

	      }else if(pGPIOx == GPIOC)
	      {
	    	  GPIOC_PCLK_EN();

	      }else if(pGPIOx == GPIOD)
	      {
	    	  GPIOD_PCLK_EN();

	      }else if(pGPIOx == GPIOE)
	      {
	    	  GPIOE_PCLK_EN();
	      }

	  }
	  else
	  {
	      if(pGPIOx == GPIOA)
	      {
	    	  GPIOA_PCLK_DIS();

	      }else if(pGPIOx == GPIOB)
	      {
	    	  GPIOB_PCLK_DIS();

	      }else if(pGPIOx == GPIOC)
	      {
	    	  GPIOC_PCLK_DIS();

	      }else if(pGPIOx == GPIOD)
	      {
	    	  GPIOD_PCLK_DIS();

	      }else if(pGPIOx == GPIOE)
	      {
	    	  GPIOE_PCLK_DIS();
	      }
	  }
}


/*
 * Initializes the GPIOx with the pGPIOHandle.
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	//currentmode = ((uint32_t)GPIO_InitStruct->GPIO_Mode) & ((uint32_t)0x0F);

	/**************************************************************************************************************/
	//INTERRUPT MODE CONFIG // CHECK FIRST BELOW CONDITION FOR INTERRUPT MODES
	if (pGPIOHandle->GPIO_PinConfig.GPIO_Mode > GPIO_MODE_AF_OD)// It means that if one of the mode is in interrupt modes or not.
	{
		/*1. Configure the triggering selection*/
		if (pGPIOHandle->GPIO_PinConfig.GPIO_Mode == GPIO_MODE_IT_RT) {
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_Pin); // Set the RTRS reg
			EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_Pin); // Clear the FTRS reg

		} else if (pGPIOHandle->GPIO_PinConfig.GPIO_Mode == GPIO_MODE_IT_FT) {
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_Pin); // Set the FTRS reg
			EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_Pin); // Clear the RTRS reg

		} else if (pGPIOHandle->GPIO_PinConfig.GPIO_Mode == GPIO_MODE_IT_RFT) {
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_Pin); // Set the RTRS reg
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_Pin); // Set the FTRS reg

		}

		/*2. Configuration of the GPIO Port Selection in AFIO->EXTICR*/
		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_Pin / 4; //Created for deciding which EXTICR register
		uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_Pin % 4; //Created for deciding which pins(4 pin) inside the selected EXTICR register

		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
		AFIO_PCLK_EN(); //Clock enabled first.
		AFIO->EXTICR[temp1] = portcode << (temp2 * 4);

		/*3. Enable the EXTI from interrupt mask register (EXTI->IMR)*/
		EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_Pin);

	}
	/**************************************************************************************************************/


	uint32_t tempmode=0;

	// 1. Configure the mode, first define whether the mode is output or not.
	if((pGPIOHandle->GPIO_PinConfig.GPIO_Mode) & (0x10))//All output mode's 5th bit is 1 (ex: 0001 0100)
	{
		pGPIOHandle->GPIO_PinConfig.GPIO_Mode |= pGPIOHandle->GPIO_PinConfig.GPIO_Speed; //Entegrating Output mode with speed
	}

	//2. Configure the mode for CRL
	if(pGPIOHandle->GPIO_PinConfig.GPIO_Pin <=7) //Below the pin number 7 is in CRL register
	{

	    /* Reset the corresponding ODR bit to configure pull-down activation */
	    if (pGPIOHandle->GPIO_PinConfig.GPIO_Mode == GPIO_MODE_INPUT_PD)
	    {
	    	pGPIOHandle->pGPIOx->BRR &= ~( 1 << pGPIOHandle->GPIO_PinConfig.GPIO_Pin);
	    	pGPIOHandle->pGPIOx->BRR |= ( 1 << (pGPIOHandle->GPIO_PinConfig.GPIO_Pin));
	    }
	    else
	    {
	      /* Set the corresponding ODR bit to configure pull-up activation */
	      if (pGPIOHandle->GPIO_PinConfig.GPIO_Mode == GPIO_MODE_INPUT_PU)
	      {
	  		  pGPIOHandle->pGPIOx->BSRR &= ~( 1 << pGPIOHandle->GPIO_PinConfig.GPIO_Pin); // Clearing the corresponding bits.
	    	  pGPIOHandle->pGPIOx->BSRR |= ( 1 << (pGPIOHandle->GPIO_PinConfig.GPIO_Pin));
	      }
	    }

		tempmode |= (((pGPIOHandle->GPIO_PinConfig.GPIO_Mode) & (0x0F)) <<  (4 * pGPIOHandle->GPIO_PinConfig.GPIO_Pin) ); // Getting the last 4 bits of GPIO_Mode to shift them 4 times left properly
		pGPIOHandle->pGPIOx->CRL &= ~( 0xF << pGPIOHandle->GPIO_PinConfig.GPIO_Pin); // Clearing the corresponding bits first.
		pGPIOHandle->pGPIOx->CRL |= tempmode;
	}


	//3. Configure the mode for CRH
	if(pGPIOHandle->GPIO_PinConfig.GPIO_Pin >7) //Above the pin number 7 is in CRH register
	{
		tempmode=0;

	    /* Reset the corresponding ODR bit to configure pull-down activation(Told in Reference Manual) */
	    if (pGPIOHandle->GPIO_PinConfig.GPIO_Mode == GPIO_MODE_INPUT_PD)
	    {
	    	pGPIOHandle->pGPIOx->BRR &= ~( 1 << pGPIOHandle->GPIO_PinConfig.GPIO_Pin); // Clearing the corresponding bits.
	    	pGPIOHandle->pGPIOx->BRR |= ( 1 << (pGPIOHandle->GPIO_PinConfig.GPIO_Pin));
	    }
	    else
	    {
	      /* Set the corresponding ODR bit to configure pull-up activation(Told in Reference Manual) */
	      if (pGPIOHandle->GPIO_PinConfig.GPIO_Mode == GPIO_MODE_INPUT_PU)
	      {
	    	  pGPIOHandle->pGPIOx->BSRR &= ~( 1 << pGPIOHandle->GPIO_PinConfig.GPIO_Pin); // Clearing the corresponding bits.
	    	  pGPIOHandle->pGPIOx->BSRR |= ( 1 << (pGPIOHandle->GPIO_PinConfig.GPIO_Pin));
	      }
	    }

		tempmode |= (((pGPIOHandle->GPIO_PinConfig.GPIO_Mode) & (0x0F)) <<  (4 * ((pGPIOHandle->GPIO_PinConfig.GPIO_Pin) - 8)) ); // Getting the last 4 bits of GPIO_Mode to shift them 4 times left properly. Between 8-16 pin number
		pGPIOHandle->pGPIOx->CRH &= ~( 0xF << pGPIOHandle->GPIO_PinConfig.GPIO_Pin); // Clearing the corresponding bits first.
		pGPIOHandle->pGPIOx->CRH |= tempmode ;

	}

}



/*
 * De-initializes(reset) the GPIOx peripheral registers with using RCC reset registers.
 */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{

	if (pGPIOx == GPIOA)
	{
		GPIOA_REG_RESET();  /* GPIO port A clock reset */

	}else if (pGPIOx == GPIOB)
	{
		GPIOB_REG_RESET();	/* GPIO port B clock reset */

	}else if (pGPIOx == GPIOC)
	{
		GPIOC_REG_RESET();	/* GPIO port C clock reset */

	}else if (pGPIOx == GPIOD)
	{
		GPIOD_REG_RESET();	/* GPIO port D clock reset */

	}else if (pGPIOx == GPIOE)
	{
		GPIOE_REG_RESET();	/* GPIO port E clock reset */
	}

}



uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value=0;
	value = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001);// Shift the corresponding reg value amount of pin number
													  //and take the last bit with AND operator(masking)
	return value;

}

uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint32_t value=0;
	value = (uint16_t)pGPIOx->IDR; //Typecaste to 16 bit only and take all the value

	return value;
}

void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
	{
		pGPIOx->ODR |= (1<<PinNumber);

	}else
	{
		pGPIOx->ODR &= ~(1<<PinNumber);

	}
}

void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value)
{
	pGPIOx->ODR = Value;

}

void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= (1<<PinNumber);

}


/*
 * NVIC register Configurations for Interrupts.
 * We have less than 96 IRQ numbers in this device header. First 3 register of below registers is enough for us.
 * IRQ(Interrupt Request) Number definitions are in stm32f1xx.h(device header file)
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDis)
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


void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// Find out the IPR(Interrupt Priority Register)
	uint8_t iprx = IRQNumber / 4; //each iprx is 8 bit, 32 bit is divided by 4
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8- NO_PR_BITS_IMPLEMENTED); //PR=Priority & NO_PR_BITS_IMPLEMENTED=4 // Every 8 bits of iprx's first 4 bit is not used. So, we just left shifted 4 times. Thats it

	*(NVIC_PR_BASE_ADDR +  iprx ) |= ( IRQPriority << shift_amount ) ;

}



/*
 * IRQ Handling function will use inside the actual IRQ_Handling
 * function implemented as weak in startup file.
 */
void GPIO_IRQHandling(uint8_t PinNumber)
{
	//Clear the EXTI PR Register corresponding to the pin number. So that interrrupt could occurs again
	if(EXTI->PR & (1 << PinNumber))
	{
		EXTI->PR |=  (1 << PinNumber);
	}


}

