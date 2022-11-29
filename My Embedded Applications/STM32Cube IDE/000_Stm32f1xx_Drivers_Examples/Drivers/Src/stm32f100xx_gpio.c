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

void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDis)
{

}

void GPIO_IRQHandling(uint8_t PinNumber)
{

}

