/*
 * stm32f100xx_rcc.c
 *
 *  Created on: 8 Mar 2023
 *      Author: lutfullah_aydin
 */


/******************************************************************************/
/*            ##### RCC Peripheral Module Driver(Simple Usage) #####	      */
/******************************************************************************/


#include "stm32f100xx_rcc.h"



uint16_t AHB_PreScaler[9] = {2,4,8,16,32,64,128,256,512};
uint8_t APB1_PreScaler[4] = { 2, 4 , 8, 16};


/*
 * Generating PERIPH CLOCK value for APB1 Bus
 */
uint32_t RCC_GetPCLK1Value(void) // Verify this function from Datasheet & Reference Man
{
	uint32_t pclk1,SystemClk;
	uint16_t ahbp;//AHB Prescaler
	uint8_t clksrc, temp, apb1p; //AHB Prescaler, APB1 Prescaler

	/*
	SWS: System clock switch status
	Set and cleared by hardware to indicate which clock source is used as system clock.
	00: HSI oscillator used as system clock
	01: HSE oscillator used as system clock
	10: PLL used as system clock
	11: not applicable
	*/
	clksrc = ((RCC->CFGR >> 2) & 0x3); // Bringing those 2 bits to lsb position and mask
	if(clksrc == 0) // Look at the reference manual
	{
		SystemClk = 16000000;
	}else if(clksrc == 1)
	{
		SystemClk = 8000000;
	}else if (clksrc == 2)
	{
		SystemClk = RCC_GetPLLOutputClock(); // PLL used as system clock
	}

	//for AHB
	temp = ((RCC->CFGR >> 4 ) & 0xF); // Bringing those 4 bits to lsb position and mask
	if(temp < 8)// Look at the reference manual
	{
		ahbp = 1;
	}else
	{
		ahbp = AHB_PreScaler[temp-8];
	}



	//APB1
	temp = ((RCC->CFGR >> 10 ) & 0x7);// Bringing those 3 bits to lsb position and mask
	if(temp < 4)// Look at the reference manual
	{
		apb1p = 1;
	}else
	{
		apb1p = APB1_PreScaler[temp-4];
	}

	pclk1 =  (SystemClk / ahbp) /apb1p;

	return pclk1;
}



/*
 * Generating PERIPH CLOCK value for APB1 Bus
 */
uint32_t RCC_GetPCLK2Value(void)
{
	uint32_t SystemClock=0,tmp,pclk2;
	uint8_t clk_src = ( RCC->CFGR >> 2) & 0X3;

	uint8_t ahbp,apb2p;

	if(clk_src == 0)
	{
		SystemClock = 16000000;
	}else
	{
		SystemClock = 8000000;
	}
	tmp = (RCC->CFGR >> 4 ) & 0xF;

	if(tmp < 0x08)
	{
		ahbp = 1;
	}else
	{
       ahbp = AHB_PreScaler[tmp-8];
	}

	tmp = (RCC->CFGR >> 13 ) & 0x7;
	if(tmp < 0x04)
	{
		apb2p = 1;
	}else
	{
		apb2p = APB1_PreScaler[tmp-4];
	}

	pclk2 = (SystemClock / ahbp )/ apb2p;

	return pclk2;
}

/*
 * We are not going to use PLL for system clock. This is only a demonstration.
 */
uint32_t  RCC_GetPLLOutputClock()
{
	return 0;
}


