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

#define  GPIO_MODE_IT_RT        0x10110000u   /*External Interrupt Mode with Rising edge trigger         */
#define  GPIO_MODE_IT_FT        0x10210000u   /*External Interrupt Mode with Falling edge trigger        */
#define  GPIO_MODE_IT_RFT	    0x10310000u   /*External Interrupt Mode with Rising&Falling edge trigger */


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


/*
 * GPIO Remap Definition
 */
#define GPIO_Remap_SPI1             ((uint32_t)0x00000001)  /*!< SPI1 Alternate Function mapping */
#define GPIO_Remap_I2C1             ((uint32_t)0x00000002)  /*!< I2C1 Alternate Function mapping */
#define GPIO_Remap_USART1           ((uint32_t)0x00000004)  /*!< USART1 Alternate Function mapping */
#define GPIO_Remap_USART2           ((uint32_t)0x00000008)  /*!< USART2 Alternate Function mapping */
#define GPIO_PartialRemap_USART3    ((uint32_t)0x00140010)  /*!< USART3 Partial Alternate Function mapping */
#define GPIO_FullRemap_USART3       ((uint32_t)0x00140030)  /*!< USART3 Full Alternate Function mapping */
#define GPIO_PartialRemap_TIM1      ((uint32_t)0x00160040)  /*!< TIM1 Partial Alternate Function mapping */
#define GPIO_FullRemap_TIM1         ((uint32_t)0x001600C0)  /*!< TIM1 Full Alternate Function mapping */
#define GPIO_PartialRemap1_TIM2     ((uint32_t)0x00180100)  /*!< TIM2 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_TIM2     ((uint32_t)0x00180200)  /*!< TIM2 Partial2 Alternate Function mapping */
#define GPIO_FullRemap_TIM2         ((uint32_t)0x00180300)  /*!< TIM2 Full Alternate Function mapping */
#define GPIO_PartialRemap_TIM3      ((uint32_t)0x001A0800)  /*!< TIM3 Partial Alternate Function mapping */
#define GPIO_FullRemap_TIM3         ((uint32_t)0x001A0C00)  /*!< TIM3 Full Alternate Function mapping */
#define GPIO_Remap_TIM4             ((uint32_t)0x00001000)  /*!< TIM4 Alternate Function mapping */
#define GPIO_Remap1_CAN1            ((uint32_t)0x001D4000)  /*!< CAN1 Alternate Function mapping */
#define GPIO_Remap2_CAN1            ((uint32_t)0x001D6000)  /*!< CAN1 Alternate Function mapping */
#define GPIO_Remap_PD01             ((uint32_t)0x00008000)  /*!< PD01 Alternate Function mapping */
#define GPIO_Remap_TIM5CH4_LSI      ((uint32_t)0x00200001)  /*!< LSI connected to TIM5 Channel4 input capture for calibration */
#define GPIO_Remap_ADC1_ETRGINJ     ((uint32_t)0x00200002)  /*!< ADC1 External Trigger Injected Conversion remapping */
#define GPIO_Remap_ADC1_ETRGREG     ((uint32_t)0x00200004)  /*!< ADC1 External Trigger Regular Conversion remapping */
#define GPIO_Remap_ADC2_ETRGINJ     ((uint32_t)0x00200008)  /*!< ADC2 External Trigger Injected Conversion remapping */
#define GPIO_Remap_ADC2_ETRGREG     ((uint32_t)0x00200010)  /*!< ADC2 External Trigger Regular Conversion remapping */
#define GPIO_Remap_ETH              ((uint32_t)0x00200020)  /*!< Ethernet remapping (only for Connectivity line devices) */
#define GPIO_Remap_CAN2             ((uint32_t)0x00200040)  /*!< CAN2 remapping (only for Connectivity line devices) */
#define GPIO_Remap_SWJ_NoJTRST      ((uint32_t)0x00300100)  /*!< Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST */
#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)  /*!< JTAG-DP Disabled and SW-DP Enabled */
#define GPIO_Remap_SWJ_Disable      ((uint32_t)0x00300400)  /*!< Full SWJ Disabled (JTAG-DP + SW-DP) */
#define GPIO_Remap_SPI3             ((uint32_t)0x00201100)  /*!< SPI3/I2S3 Alternate Function mapping (only for Connectivity line devices) */
#define GPIO_Remap_TIM2ITR1_PTP_SOF ((uint32_t)0x00202000)  /*!< Ethernet PTP output or USB OTG SOF (Start of Frame) connected
                                                                 to TIM2 Internal Trigger 1 for calibration
                                                                 (only for Connectivity line devices) */
#define GPIO_Remap_PTP_PPS          ((uint32_t)0x00204000)  /*!< Ethernet MAC PPS_PTS output on PB05 (only for Connectivity line devices) */

#define GPIO_Remap_TIM15            ((uint32_t)0x80000001)  /*!< TIM15 Alternate Function mapping (only for Value line devices) */
#define GPIO_Remap_TIM16            ((uint32_t)0x80000002)  /*!< TIM16 Alternate Function mapping (only for Value line devices) */
#define GPIO_Remap_TIM17            ((uint32_t)0x80000004)  /*!< TIM17 Alternate Function mapping (only for Value line devices) */
#define GPIO_Remap_CEC              ((uint32_t)0x80000008)  /*!< CEC Alternate Function mapping (only for Value line devices) */
#define GPIO_Remap_TIM1_DMA         ((uint32_t)0x80000010)  /*!< TIM1 DMA requests mapping (only for Value line devices) */

#define GPIO_Remap_TIM9             ((uint32_t)0x80000020)  /*!< TIM9 Alternate Function mapping (only for XL-density devices) */
#define GPIO_Remap_TIM10            ((uint32_t)0x80000040)  /*!< TIM10 Alternate Function mapping (only for XL-density devices) */
#define GPIO_Remap_TIM11            ((uint32_t)0x80000080)  /*!< TIM11 Alternate Function mapping (only for XL-density devices) */
#define GPIO_Remap_TIM13            ((uint32_t)0x80000100)  /*!< TIM13 Alternate Function mapping (only for High density Value line and XL-density devices) */
#define GPIO_Remap_TIM14            ((uint32_t)0x80000200)  /*!< TIM14 Alternate Function mapping (only for High density Value line and XL-density devices) */
#define GPIO_Remap_FSMC_NADV        ((uint32_t)0x80000400)  /*!< FSMC_NADV Alternate Function mapping (only for High density Value line and XL-density devices) */

#define GPIO_Remap_TIM67_DAC_DMA    ((uint32_t)0x80000800)  /*!< TIM6/TIM7 and DAC DMA requests remapping (only for High density Value line devices) */
#define GPIO_Remap_TIM12            ((uint32_t)0x80001000)  /*!< TIM12 Alternate Function mapping (only for High density Value line devices) */
#define GPIO_Remap_MISC             ((uint32_t)0x80002000)  /*!< Miscellaneous Remap (DMA2 Channel5 Position and DAC Trigger remapping,
                                                                 only for High density Value line devices) */



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

void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDis);/*IRQ Configuration(Enable or Disable)*/
void GPIO_IRQHandling(uint8_t PinNumber); /*ISR(Interrupt Service Routine) Handling*/








#endif /* INC_STM32F100XX_GPIO_H_ */
