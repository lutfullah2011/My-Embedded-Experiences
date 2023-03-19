/*
 * stm32f100xx_rcc.h
 *
 *  Created on: 8 Mar 2023
 *      Author: lutfullah_aydin
 */

#ifndef INC_STM32F100XX_RCC_H_
#define INC_STM32F100XX_RCC_H_

#include "stm32f1xx.h"


//This returns the APB1 clock value
uint32_t RCC_GetPCLK1Value(void);

//This returns the APB2 clock value
uint32_t RCC_GetPCLK2Value(void);

//This is used when PLL is used as a system clock
uint32_t  RCC_GetPLLOutputClock(void);



#endif /* INC_STM32F100XX_RCC_H_ */
