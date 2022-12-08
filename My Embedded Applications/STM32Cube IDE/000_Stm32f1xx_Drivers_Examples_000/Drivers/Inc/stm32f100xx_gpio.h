/*
 * stm32f100xx_gpio.h
 *
 *  Created on: 15 Kas 2022
 *      Author: lutfullah_aydin
 */

#ifndef INC_STM32F100XX_GPIO_H_
#define INC_STM32F100XX_GPIO_H_


#include "stm32f1xx.h"

/******************************************************************************/
/*                  Structures for GPIO pin				    	          */
/******************************************************************************/


/*
 * Configuration Structure for GPIO pin
 */
typedef struct
{
  uint32_t GPIO_Pin;       		/* Specifies the GPIO pin to be configured */

  uint32_t GPIO_Mode;  		    /* Specifies the operating mode for the pin.
  	  	  	  	  	  	  	  	   This parameter can be a value of @ref GPIO_MODES */

  uint32_t GPIO_Speed;	        /* Specifies the speed for the pin.*/

}GPIO_PinConfig_t;



/*
 * Handle Structure for GPIO pin
 */
typedef struct
{
	GPIO_RegDef_t *pGPIOx;				/*This holds the base address of GPIO Port*/
	GPIO_PinConfig_t GPIO_PinConfig;	/*This holds GPIO pin configuration settings*/

}GPIO_Handle_t;


/*
 * GPIO Speed Configuration with assigning a value
 */
#define  GPIO_SPEED_LOW        1 	   /* Low speed = 2MHz */
#define  GPIO_SPEED_MEDIUM     2       /* Medium speed = 10MHz */
#define  GPIO_SPEED_HIGH       3       /* High speed = 50MHz */


/*
 * @ref GPIO_MODES
 * GPIO Mode Configuration with assigning a value
 * Last 2 bits are zero(00) intentionaly and indicates == Speed config for Output modes, 00 for Input modes.
 * Output mode's 5th bit intentionaly 1 to separate from input modes.
 */
#define	 GPIO_MODE_ANALOG 		0x00 				/*0000 0000*/	/*Analog Input*/
#define  GPIO_MODE_INPUT  		0x04				/*0000 0100*/	/*Input Floating(Default)*/
#define  GPIO_MODE_INPUT_PD  	0x28				/*0010 1000*/	/*Input Pull-down*/
#define  GPIO_MODE_INPUT_PU  	0x48				/*0100 1000*/	/*Input Pull-up*/
#define  GPIO_MODE_OUT_PP  		0x10				/*0001 0000*/	/*Output Push-Pull */
#define  GPIO_MODE_OUT_OD  		0x14				/*0001 0100*/	/*Output Open-Drain */
#define  GPIO_MODE_AF_PP  		0x18				/*0001 1000*/	/*Alternate Function Push-Pull*/
#define  GPIO_MODE_AF_OD  		0x1C				/*0001 1100*/	/*Alternate Function Open-Drain*/

#define  GPIO_MODE_IT_RT        0x24 				/*0010 0100*/   /*External Interrupt Mode with Rising edge trigger         */
#define  GPIO_MODE_IT_FT        0x44 				/*0100 0100*/   /*External Interrupt Mode with Falling edge trigger        */
#define  GPIO_MODE_IT_RFT	    0x84 				/*1000 0100*/   /*External Interrupt Mode with Rising&Falling edge trigger */


/*
 * GPIO Pin Definition
 */
#define GPIO_Pin_0                 0		/* Pin 0 selected */
#define GPIO_Pin_1                 1   		/* Pin 1 selected */
#define GPIO_Pin_2                 2   		/* Pin 2 selected */
#define GPIO_Pin_3                 3   		/* Pin 3 selected */
#define GPIO_Pin_4                 4   		/* Pin 4 selected */
#define GPIO_Pin_5                 5   		/* Pin 5 selected */
#define GPIO_Pin_6                 6   		/* Pin 6 selected */
#define GPIO_Pin_7                 7   		/* Pin 7 selected */
#define GPIO_Pin_8                 8   		/* Pin 8 selected */
#define GPIO_Pin_9                 9  		/* Pin 9 selected */
#define GPIO_Pin_10                10  		/* Pin 10 selected */
#define GPIO_Pin_11                11  		/* Pin 11 selected */
#define GPIO_Pin_12                12  		/* Pin 12 selected */
#define GPIO_Pin_13                13  		/* Pin 13 selected */
#define GPIO_Pin_14                14  		/* Pin 14 selected */
#define GPIO_Pin_15                15  		/* Pin 15 selected */




/******************************************************************************/
/*                  Function Prototype by this driver - APIs	              */
/******************************************************************************/

void GPIO_PCLK(GPIO_RegDef_t *pGPIOx, uint8_t EnorDis);/*Peripheral Clock Setup(Enable or Disable)*/

void GPIO_Init(GPIO_Handle_t *pGPIOHandle);/*Peripheral Initialization*/
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);/*Peripheral Deinitialization*/

uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);   /*Peripheral Data read for pin*/
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);  /*Peripheral Data read for port*/
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);	/*Peripheral Data write for pin*/
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);	/*Peripheral Data write for port*/
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber); /*Peripheral Toggle output*/

void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDis);/*IRQ Interrupt Configuration(Enable or Disable)*/
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority); /*IRQ Priority Configuration*/
void GPIO_IRQHandling(uint8_t PinNumber); /*ISR(Interrupt Service Routine) Handling*/








#endif /* INC_STM32F100XX_GPIO_H_ */
