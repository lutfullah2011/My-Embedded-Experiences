/*
 * stm32f1xx.h
 *
 *  Created on: Nov 12, 2022
 *      Author: lutfullah_aydin
 */

#ifndef INC_STM32F1XX_H_
#define INC_STM32F1XX_H_

#include <stdint.h>
#include<stddef.h>

#define __vo volatile
#define __weak __attribute__((weak))

/******************************************************************************/
/*                   START:Processor Specific Details		                  */
/******************************************************************************/

/*
 * ARM Cortex M3 Processor NVIC ISERx (Interrupt Set-Enable Register) Register Addresses
 */
#define NVIC_ISER0						( (__vo uint32_t*)0xE000E100 )
#define NVIC_ISER1						( (__vo uint32_t*)0xE000E104 )
#define NVIC_ISER2						( (__vo uint32_t*)0xE000E108 )


/*
 * ARM Cortex M3 Processor NVIC ICERx (Interrupt Clear-Enable Register) Register Addresses
 */
#define NVIC_ICER0						( (__vo uint32_t*)0XE000E180 )
#define NVIC_ICER1						( (__vo uint32_t*)0XE000E184 )
#define NVIC_ICER2						( (__vo uint32_t*)0XE000E188 )


/*
 * ARM Cortex M3 Processor NVIC IPR (Interrupt Priority Register) Register Base Address
 */
#define NVIC_PR_BASE_ADDR				( (__vo uint32_t*)0xE000E400 )


#define NO_PR_BITS_IMPLEMENTED			4 // Number of Priority bits implemented for this device is 4. Used while setting IRQ priority number



/******************************************************************************/
/*                   		  	  MEMORY MAP					              */
/******************************************************************************/

/*
 * Memory Base adresses
 */
#define FLASH_BASEADDR				0x08000000U /*Main Memory*/
#define SRAM1_BASEADDR				0x20000000U
#define SRAM_BASEADDR			    SRAM1_BASEADDR /*SRAM1 is the main SRAM*/


/*
 * Bus Base adresses
 */
#define PERIPH_BASEADDR				0x40000000U /*Peripheral base adresses*/
#define APB1PERIPH_BASEADDR			PERIPH_BASEADDR
#define APB2PERIPH_BASEADDR			0x40010000U
#define AHBPERIPH_BASEADDR			0x40020000U


/*
 * Base adresses for Peripheral APB1, APB2 and AHB
 */
#define I2C1_BASEADDR				(APB1PERIPH_BASEADDR + 0x5400)

#define AFIO_BASEADDR				(APB2PERIPH_BASEADDR + 0x0000)
#define EXTI_BASEADDR				(APB2PERIPH_BASEADDR + 0x0400)
#define GPIOA_BASEADDR				(APB2PERIPH_BASEADDR + 0x0800)
#define GPIOB_BASEADDR				(APB2PERIPH_BASEADDR + 0x0C00)
#define GPIOC_BASEADDR				(APB2PERIPH_BASEADDR + 0x1000)
#define GPIOD_BASEADDR				(APB2PERIPH_BASEADDR + 0x1400)
#define GPIOE_BASEADDR				(APB2PERIPH_BASEADDR + 0x1800)
#define ADC1_BASEADDR				(APB2PERIPH_BASEADDR + 0x2400)
#define SPI1_BASEADDR				(APB2PERIPH_BASEADDR + 0x3000)
#define USART1_BASEADDR				(APB2PERIPH_BASEADDR + 0x3800)

#define DMA1_BASEADDR				(AHBPERIPH_BASEADDR + 0x0000)
#define DMA1_Channel1_BASEADDR    	(AHBPERIPH_BASEADDR + 0x0008)
#define DMA1_Channel2_BASEADDR      (AHBPERIPH_BASEADDR + 0x001C)
#define DMA1_Channel3_BASEADDR      (AHBPERIPH_BASEADDR + 0x0030)
#define DMA1_Channel4_BASEADDR      (AHBPERIPH_BASEADDR + 0x0044)
#define DMA1_Channel5_BASEADDR      (AHBPERIPH_BASEADDR + 0x0058)
#define DMA1_Channel6_BASEADDR      (AHBPERIPH_BASEADDR + 0x006C)
#define DMA1_Channel7_BASEADDR      (AHBPERIPH_BASEADDR + 0x0080)
#define RCC_BASEADDR				(AHBPERIPH_BASEADDR + 0x1000)




/******************************************************************************/
/*                   Peripheral Registers Definition Structures               */
/******************************************************************************/

/*
 * General Purpose I/O
 */
typedef struct
{
	__vo uint32_t CRL;			/* Configuration register low	 Adress offset: 0x00 */
    __vo uint32_t CRH;			/* Configuration register high	 Adress offset: 0x04 */
    __vo uint32_t IDR;			/* Input data register			 Adress offset: 0x08 */
    __vo uint32_t ODR;			/* Output data register			 Adress offset: 0x0C */
    __vo uint32_t BSRR;			/* Bit set/reset register		 Adress offset: 0x10 */
    __vo uint32_t BRR;			/* Bit reset register			 Adress offset: 0x14 */
    // I do not use it for now //__vo uint32_t LCKR;			/* Configuration lock register	 Adress offset: 0x18 */
} GPIO_RegDef_t;


/*
 * Alternate Function I/O
 */
typedef struct
{
    __vo uint32_t EVCR;			/* Event control register	 							Adress offset: 0x00 */
    __vo uint32_t MAPR;			/* Remap and debug I/O configuration register			Adress offset: 0x04 */
    __vo uint32_t EXTICR[4];	/* External interrupt configuration register 1-2-3-4	Adress offset: 0x08-0x14 */
    uint32_t RESERVED0;			/* Reserved												Adress offset: 0x18 */
    __vo uint32_t MAPR2;		/* Remap and debug I/O configuration register 2			Adress offset: 0x1C */
} AFIO_RegDef_t;


/*
 * External Interrupt/Event Controller
 */

typedef struct
{
	__vo uint32_t IMR;			/* Interrupt Mask register 	 		 		Adress offset: 0x00 */
	__vo uint32_t EMR;			/* Event Mask register 	 		 			Adress offset: 0x04 */
	__vo uint32_t RTSR;			/* Rising Trigger selection register 	 	Adress offset: 0x08 */
	__vo uint32_t FTSR;			/* Falling Trigger selection register 	 	Adress offset: 0x0C */
	__vo uint32_t SWIER;		/* Software Interrupt event register 	 	Adress offset: 0x10 */
	__vo uint32_t PR;			/* Pending register 	 		 			Adress offset: 0x14 */
} EXTI_RegDef_t;


/*
 * Reset and Clock Control
 */
typedef struct
{
    __vo uint32_t CR;			/* Clock Control register 	 		 		Adress offset: 0x00 */
    __vo uint32_t CFGR;			/* Clock Configuration register 	 		Adress offset: 0x04 */
    __vo uint32_t CIR;			/* Clock interrupt register 	 	 		Adress offset: 0x08 */
    __vo uint32_t APB2RSTR;		/* APB2 peripheral reset register 	 		Adress offset: 0x0C */
    __vo uint32_t APB1RSTR;		/* APB1 peripheral reset register 	 		Adress offset: 0x10 */
    __vo uint32_t AHBENR;		/* AHB peripheral clock enable register		Adress offset: 0x14 */
    __vo uint32_t APB2ENR;		/* APB2 peripheral clock enable register	Adress offset: 0x18 */
    __vo uint32_t APB1ENR;		/* APB1 peripheral clock enable register	Adress offset: 0x1C */
    __vo uint32_t BDCR;			/* Backup domain control register 	 		Adress offset: 0x20 */
    __vo uint32_t CSR;			/* Control/status register 	 				Adress offset: 0x24 */

    uint32_t RESERVED0;			/* Reserved	 	 							Adress offset: 0x28 */
    __vo uint32_t CFGR2;		/* Clock configuration register 2 	 		Adress offset: 0x2C */
} RCC_RegDef_t;


/*
 * Analog to Digital Converter
 */
typedef struct
{
	__vo uint32_t SR;			/* Status register 	 							Adress offset: 0x00 */
	__vo uint32_t CR1;			/* Control register 1 	 						Adress offset: 0x04 */
	__vo uint32_t CR2;			/* Control register 2 	 						Adress offset: 0x08 */
    __vo uint32_t SMPR1;		/* Sample time register 1 	 					Adress offset: 0x0C */
    __vo uint32_t SMPR2;		/* Sample time register 2 	 					Adress offset: 0x10 */
    __vo uint32_t JOFR1;		/* Injected channel data offset register 1 	 	Adress offset: 0x14 */
    __vo uint32_t JOFR2;		/* Injected channel data offset register 2 	 	Adress offset: 0x18 */
    __vo uint32_t JOFR3;		/* Injected channel data offset register 3 	 	Adress offset: 0x1C */
    __vo uint32_t JOFR4;		/* Injected channel data offset register 4 	 	Adress offset: 0x20 */
    __vo uint32_t HTR;			/* Watchdog high threshold register 	 		Adress offset: 0x24 */
    __vo uint32_t LTR;			/* Watchdog low threshold register 	 			Adress offset: 0x28 */
    __vo uint32_t SQR1;			/* Regular sequence register 1 	 				Adress offset: 0x2C */
    __vo uint32_t SQR2;			/* Regular sequence register 2 	 				Adress offset: 0x30 */
    __vo uint32_t SQR3;			/* Regular sequence register 3 	 				Adress offset: 0x34 */
    __vo uint32_t JSQR;			/* Injected sequence register 	 				Adress offset: 0x38 */
    __vo uint32_t JDR1;			/* Injected data register 1 	 				Adress offset: 0x3C */
    __vo uint32_t JDR2;			/* Injected data register 2 	 				Adress offset: 0x40 */
    __vo uint32_t JDR3;			/* Injected data register 3 	 				Adress offset: 0x44 */
    __vo uint32_t JDR4;			/* Injected data register 4 	 				Adress offset: 0x48 */
    __vo uint32_t DR;			/* Regular data register	 					Adress offset: 0x4C */
} ADC_RegDef_t;


/*
 * Universal Synchronous Asynchronous Receiver Transmitter
 */
typedef struct
{
	__vo uint32_t SR;         	/* Status register                   Address offset: 0x00 */
	__vo uint32_t DR;         	/* Data register                     Address offset: 0x04 */
	__vo uint32_t BRR;        	/* Baud rate register                Address offset: 0x08 */
	__vo uint32_t CR1;        	/* Control register 1                Address offset: 0x0C */
	__vo uint32_t CR2;        	/* Control register 2                Address offset: 0x10 */
	__vo uint32_t CR3;        	/* Control register 3                Address offset: 0x14 */
	__vo uint32_t GTPR;       	/* Guard time and prescaler register Address offset: 0x18 */
} USART_RegDef_t;


/*
 * Inter Integrated Circuit Interface
 */
typedef struct
{
	__vo uint32_t CR1;			/* Control register 1                Address offset: 0x00 */
	__vo uint32_t CR2;			/* Control register 2                Address offset: 0x04 */
	__vo uint32_t OAR1;			/* Own address register 1            Address offset: 0x08 */
	__vo uint32_t OAR2;			/* Own address register 2            Address offset: 0x0C */
	__vo uint32_t DR;			/* Data register 1                	 Address offset: 0x10 */
	__vo uint32_t SR1;			/* Status register 1                 Address offset: 0x14 */
	__vo uint32_t SR2;			/* Status register 2                 Address offset: 0x18 */
	__vo uint32_t CCR;			/* Clock control register            Address offset: 0x1C */
	__vo uint32_t TRISE;		/* TRISE(Rise time) register         Address offset: 0x20 */
} I2C_RegDef_t;


/*
 * Serial Peripheral Interface
 */
typedef struct
{
	__vo uint32_t CR1;			/* Control register 1                Address offset: 0x00 */
	__vo uint32_t CR2;			/* Control register 2                Address offset: 0x04 */
	__vo uint32_t SR;			/* Status register                	 Address offset: 0x08 */
	__vo uint32_t DR;			/* Data register                 	 Address offset: 0x0C */
	__vo uint32_t CRCPR;		/* CRC polynomial register			 Address offset: 0x10 */ /*CRC(Cyclic redundancy check)*/
	__vo uint32_t RXCRCR;		/* RX CRC register                 	 Address offset: 0x14 */ /*CRC = Döngü fazlalık denetimi*/
	__vo uint32_t TXCRCR;		/* TX CRC register                   Address offset: 0x18 */
} SPI_RegDef_t;


/*
 * DMA Controller
 */
typedef struct
{
	__vo uint32_t CCR;			/* Channel x configuration register             Address offset: 0x08 + 0d20 × (channel number – 1) */
    __vo uint32_t CNDTR;		/* Channel x number of data register            Address offset: 0x0C + 0d20 × (channel number – 1) */
    __vo uint32_t CPAR;			/* Channel x peripheral address register    	Address offset: 0x10 + 0d20 × (channel number – 1) */
    __vo uint32_t CMAR;			/* Channel x memory address register            Address offset: 0x14 + 0d20 × (channel number – 1) */
} DMA_Channel_RegDef_t;

typedef struct
{
    __vo uint32_t ISR;			/* Interupt status register                 	Address offset: 0x00 */
    __vo uint32_t IFCR;			/* Interrupt flag clear register                Address offset: 0x04 */
} DMA_RegDef_t;



/******************************************************************************/
/* Peripheral Declaration(Peripheral base adresses typcasted to xxx_RegDef_t) */
/******************************************************************************/

#define GPIOA					 ((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB					 ((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC					 ((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOD					 ((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOE					 ((GPIO_RegDef_t*)GPIOE_BASEADDR)


#define AFIO                	 ((AFIO_RegDef_t*)AFIO_BASEADDR)
#define EXTI                	 ((EXTI_RegDef_t*)EXTI_BASEADDR)

#define ADC1                	 ((ADC_RegDef_t *)ADC1_BASEADDR)
#define I2C1                	 ((I2C_RegDef_t *)I2C1_BASEADDR)
#define SPI1                	 ((SPI_RegDef_t *)SPI1_BASEADDR)
#define USART1              	 ((USART_RegDef_t *)USART1_BASEADDR)
#define RCC              	 	 ((RCC_RegDef_t *) RCC_BASEADDR)

#define DMA1                	 ((DMA_RegDef_t *)DMA1_BASE)
#define DMA1_Channel1       	 ((DMA_Channel_RegDef_t *)DMA1_Channel1_BASEADDR)
#define DMA1_Channel2       	 ((DMA_Channel_RegDef_t *)DMA1_Channel1_BASEADDR)
#define DMA1_Channel3       	 ((DMA_Channel_RegDef_t *)DMA1_Channel1_BASEADDR)
#define DMA1_Channel4       	 ((DMA_Channel_RegDef_t *)DMA1_Channel1_BASEADDR)
#define DMA1_Channel5       	 ((DMA_Channel_RegDef_t *)DMA1_Channel1_BASEADDR)
#define DMA1_Channel6       	 ((DMA_Channel_RegDef_t *)DMA1_Channel1_BASEADDR)
#define DMA1_Channel7       	 ((DMA_Channel_RegDef_t *)DMA1_Channel1_BASEADDR)


/******************************************************************************/
/*                   		Clock Enable, Disable and RESET MACROS				                  */
/******************************************************************************/

/*
 * Enable Clock
 */
#define GPIOA_PCLK_EN()			(RCC->APB2ENR |= (1<<2))  /* GPIO port A clock enable */
#define GPIOB_PCLK_EN()			(RCC->APB2ENR |= (1<<3))  /* GPIO port B clock enable */
#define GPIOC_PCLK_EN()			(RCC->APB2ENR |= (1<<4))  /* GPIO port C clock enable */
#define GPIOD_PCLK_EN()			(RCC->APB2ENR |= (1<<5))  /* GPIO port D clock enable */
#define GPIOE_PCLK_EN()			(RCC->APB2ENR |= (1<<6))  /* GPIO port E clock enable */
#define AFIO_PCLK_EN()			(RCC->APB2ENR |= (1<<0))  /* AFIO clock enable */
#define ADC1_PCLK_EN()			(RCC->APB2ENR |= (1<<9))  /* ADC1 clock enable */
#define SPI1_PCLK_EN()			(RCC->APB2ENR |= (1<<12))  /* SPI1 clock enable */
#define USART1_PCLK_EN()		(RCC->APB2ENR |= (1<<14))  /* USART1 clock enable */

#define I2C1_PCLK_EN()			(RCC->APB1ENR |= (1<<21))  /* I2C1 clock enable */

#define DMA1_PCLK_EN()			(RCC->AHBENR |= (1<<0))  /* DMA1 clock enable */


/*
 * Disable Clock
 */
#define GPIOA_PCLK_DIS()			(RCC->APB2ENR &= ~(1<<2))  /* GPIO port A clock disable */
#define GPIOB_PCLK_DIS()			(RCC->APB2ENR &= ~(1<<3))  /* GPIO port B clock disable */
#define GPIOC_PCLK_DIS()			(RCC->APB2ENR &= ~(1<<4))  /* GPIO port C clock disable */
#define GPIOD_PCLK_DIS()			(RCC->APB2ENR &= ~(1<<5))  /* GPIO port D clock disable */
#define GPIOE_PCLK_DIS()			(RCC->APB2ENR &= ~(1<<6))  /* GPIO port E clock disable */
#define AFIO_PCLK_DIS()				(RCC->APB2ENR &= ~(1<<0))  /* AFIO clock disable */
#define ADC1_PCLK_DIS()				(RCC->APB2ENR &= ~(1<<9))  /* ADC1 clock disable */
#define SPI1_PCLK_DIS()				(RCC->APB2ENR &= ~(1<<12))  /* SPI1 port A clock disable */
#define USART1_PCLK_DIS()			(RCC->APB2ENR &= ~(1<<14))  /* USART1 port A clock disable */

#define I2C1_PCLK_DIS()				(RCC->APB1ENR &= ~(1<<21))  /* I2C1 port A clock disable */

#define DMA1_PCLK_DIS()				(RCC->AHBENR &= ~(1<<0))  /* DMA1 port A clock disable */


/*
 * Reset Clock
 */

#define GPIOA_REG_RESET()			do{(RCC->APB2RSTR |= (1<<2)); (RCC->APB2RSTR &= ~(1<<2));}while(0) /* GPIO port A clock reset(Set 1, then 0) */
#define GPIOB_REG_RESET()			do{(RCC->APB2RSTR |= (1<<3)); (RCC->APB2RSTR &= ~(1<<3));}while(0) /* GPIO port B clock reset(Set 1, then 0) */
#define GPIOC_REG_RESET()			do{(RCC->APB2RSTR |= (1<<4)); (RCC->APB2RSTR &= ~(1<<4));}while(0) /* GPIO port C clock reset(Set 1, then 0) */
#define GPIOD_REG_RESET()			do{(RCC->APB2RSTR |= (1<<5)); (RCC->APB2RSTR &= ~(1<<5));}while(0) /* GPIO port D clock reset(Set 1, then 0) */
#define GPIOE_REG_RESET()			do{(RCC->APB2RSTR |= (1<<6)); (RCC->APB2RSTR &= ~(1<<6));}while(0) /* GPIO port E clock reset(Set 1, then 0) */

#define SPI1_REG_RESET()			do{(RCC->APB2RSTR |= (1<<12)); (RCC->APB2RSTR &= ~(1<<12));}while(0) /* SPI1 clock reset(Set 1, then 0) */

#define I2C1_REG_RESET()			do{(RCC->APB1RSTR |= (1<<21)); (RCC->APB1RSTR &= ~(1<<21));}while(0) /* I2C1 clock reset(Set 1, then 0) */

#define USART1_REG_RESET()			do{(RCC->APB2RSTR |= (1<<14)); (RCC->APB2RSTR &= ~(1<<14));}while(0) /* USART1 clock reset(Set 1, then 0) */



/*
 * Returns GPIO Port Codes for given GPIOx base address.
 * Intentionaly created for Interrrupt Configurations but keeping
 * in device specific header for maybe further usage.
 *
 * This macro returns a code (between 0 to 4) for given GPIO base address(x)
 */

#define GPIO_BASEADDR_TO_CODE(x)   	     ((x) == GPIOA) ? 0:\
										 ((x) == GPIOB) ? 1:\
										 ((x) == GPIOC) ? 2:\
										 ((x) == GPIOD) ? 3:\
										 ((x) == GPIOE) ? 4:0;



/*
 * IRQ(Interrupt Request) Number definitions (Vector Table addresses)
 */
#define IRQn_EXTI0                   6      /*!< EXTI Line0 Interrupt                                 */
#define IRQn_EXTI1                   7      /*!< EXTI Line1 Interrupt                                 */
#define IRQn_EXTI2                   8      /*!< EXTI Line2 Interrupt                                 */
#define IRQn_EXTI3                   9      /*!< EXTI Line3 Interrupt                                 */
#define IRQn_EXTI4                   10     /*!< EXTI Line4 Interrupt                                 */
#define IRQn_EXTI9_5                 23     /*!< EXTI Line5-9 Interrupt 							  */
#define IRQn_EXTI15_10               40     /*!< EXTI Line10-15 Interrupt 							  */

#define IRQn_SPI1                    35      /*!< SPI1 Interrupt Number                                */
#define IRQn_SPI2                    36      /*!< SPI2 Interrupt Number                                */

#define IRQn_I2C1_EV				 31		/*!< I2C1 Event Interrupt Number                                */
#define IRQn_I2C1_ER				 32		/*!< I2C1 Error Interrupt Number                                */

#define IRQn_USART1                  37      /*!< USART1 Interrupt Number                                */
#define IRQn_USART2                  38      /*!< USART2 Interrupt Number                                */


// Macros for interrupt priority, 8 for now. You can use up to 16.
#define NVIC_IRQ_PRI0		0
#define NVIC_IRQ_PRI1		1
#define NVIC_IRQ_PRI2		2
#define NVIC_IRQ_PRI3		3
#define NVIC_IRQ_PRI4		4
#define NVIC_IRQ_PRI5		5
#define NVIC_IRQ_PRI6		6
#define NVIC_IRQ_PRI7		7




/*
 * Some Generic Macros
 */
#define DISABLE 		 0
#define ENABLE 			 !DISABLE
#define RESET 			 DISABLE
#define SET 			 !RESET
#define GPIO_PIN_SET   	 SET
#define GPIO_PIN_RESET   RESET
#define FLAG_RESET		 RESET
#define FLAG_SET		 SET



/******************************************************************************/
/* 					Bit Position definitions of SPI Peripheral				  */
/******************************************************************************/

// Bitfield for SPI_CR1
#define  SPI_CR1_CPHA                        0            /* Clock Phase */
#define  SPI_CR1_CPOL                        1            /* Clock Polarity */
#define  SPI_CR1_MSTR                        2            /* Master Selection */
#define  SPI_CR1_BR                          3            /* BR[2:0] bits (Baud Rate Control) */
#define  SPI_CR1_SPE                         6            /* SPI Enable(0 means disabled) */
#define  SPI_CR1_SSI                         8            /* Internal slave select */
#define  SPI_CR1_SSM                         9            /* Software slave management */
#define  SPI_CR1_RXONLY                      10           /* Receive only */
#define  SPI_CR1_DFF                         11           /* Data Frame Format */
#define  SPI_CR1_BIDIOE                      14           /* Output enable in bidirectional mode */
#define  SPI_CR1_BIDIMODE                    15           /* Bidirectional data mode enable */

// Bitfield for SPI_CR2
#define  SPI_CR2_RXDMAEN                     0            /* Rx Buffer DMA Enable */
#define  SPI_CR2_TXDMAEN                     1            /* Tx Buffer DMA Enable */
#define  SPI_CR2_SSOE                        2            /* SS Output Enable */
#define  SPI_CR2_ERRIE                       5            /* Error Interrupt Enable */
#define  SPI_CR2_RXNEIE                      6            /* RX buffer Not Empty Interrupt Enable */
#define  SPI_CR2_TXEIE                       7            /* Tx buffer Empty Interrupt Enable */

// Bitfield for SPI_SR
#define  SPI_SR_RXNE                         0            /* Receive buffer Not Empty */
#define  SPI_SR_TXE                          1            /* Transmit buffer Empty */
#define  SPI_SR_MODF                         5            /* Mode fault */
#define  SPI_SR_OVR                          6            /* Overrun flag */
#define  SPI_SR_BSY                          7            /* Busy flag */

// Bitfield for SPI_DR
#define  SPI_DR_DR							 0xFFFF       /* Data Register */



/******************************************************************************/
/* 					Bit Position definitions of I2C Peripheral				  */
/******************************************************************************/

//Bitfield for I2C_CR1
#define I2C_CR1_PE						0
#define I2C_CR1_NOSTRETCH  				7
#define I2C_CR1_START 					8
#define I2C_CR1_STOP  				 	9
#define I2C_CR1_ACK 				 	10
#define I2C_CR1_SWRST  				 	15


//Bitfield for I2C_CR2
#define I2C_CR2_FREQ				 	0
#define I2C_CR2_ITERREN				 	8
#define I2C_CR2_ITEVTEN				 	9
#define I2C_CR2_ITBUFEN 			    10


//Bitfield for I2C_OAR1
#define I2C_OAR1_ADD0    				 0
#define I2C_OAR1_ADD71 				 	 1
#define I2C_OAR1_ADD98  			 	 8
#define I2C_OAR1_BIT14   			 	14
#define I2C_OAR1_ADDMODE   			 	15


//Bitfield for I2C_SR1
#define I2C_SR1_SB 					 	0
#define I2C_SR1_ADDR 				 	1
#define I2C_SR1_BTF 					2
#define I2C_SR1_ADD10 					3
#define I2C_SR1_STOPF 					4
#define I2C_SR1_RXNE 					6
#define I2C_SR1_TXE 					7
#define I2C_SR1_BERR 					8
#define I2C_SR1_ARLO 					9
#define I2C_SR1_AF 					 	10
#define I2C_SR1_OVR 					11
#define I2C_SR1_TIMEOUT 				14


//Bitfield for I2C_SR2
#define I2C_SR2_MSL						0
#define I2C_SR2_BUSY 					1
#define I2C_SR2_TRA 					2
#define I2C_SR2_GENCALL 				4
#define I2C_SR2_DUALF 					7


//Bitfield for I2C_CCR
#define I2C_CCR_CCR 					 0
#define I2C_CCR_DUTY 					14
#define I2C_CCR_FS  				 	15



/******************************************************************************/
/* 					Bit Position definitions of USART Peripheral				  */
/******************************************************************************/


//Bitfield for USART_CR1
#define USART_CR1_SBK					0
#define USART_CR1_RWU 					1
#define USART_CR1_RE  					2
#define USART_CR1_TE 					3
#define USART_CR1_IDLEIE 				4
#define USART_CR1_RXNEIE  				5
#define USART_CR1_TCIE					6
#define USART_CR1_TXEIE					7
#define USART_CR1_PEIE 					8
#define USART_CR1_PS 					9
#define USART_CR1_PCE 					10
#define USART_CR1_WAKE  				11
#define USART_CR1_M 					12
#define USART_CR1_UE 					13
#define USART_CR1_OVER8  				15


//Bitfield for USART_CR2
#define USART_CR2_ADD   				0
#define USART_CR2_LBDL   				5
#define USART_CR2_LBDIE  				6
#define USART_CR2_LBCL   				8
#define USART_CR2_CPHA   				9
#define USART_CR2_CPOL   				10
#define USART_CR2_STOP   				12
#define USART_CR2_LINEN   				14


//Bitfield for USART_CR3
#define USART_CR3_EIE   				0
#define USART_CR3_IREN   				1
#define USART_CR3_IRLP  				2
#define USART_CR3_HDSEL   				3
#define USART_CR3_NACK   				4
#define USART_CR3_SCEN   				5
#define USART_CR3_DMAR  				6
#define USART_CR3_DMAT   				7
#define USART_CR3_RTSE   				8
#define USART_CR3_CTSE   				9
#define USART_CR3_CTSIE   				10
#define USART_CR3_ONEBIT   				11


//Bitfield for USART_SR
#define USART_SR_PE        				0 //Parity Error
#define USART_SR_FE        				1 //Framing Error
#define USART_SR_NF        				2 //Noise detected Flag
#define USART_SR_ORE       				3 //Overrun Error
#define USART_SR_IDLE       			4 //IDLE line Detected
#define USART_SR_RXNE        			5 //Read data register not Empty
#define USART_SR_TC        				6 //Tranmission Complete
#define USART_SR_TXE        			7 //Transmit data Register Empty
#define USART_SR_LBD        			8 //LIN break detection flag
#define USART_SR_CTS        			9 //CTS flag



//Include files All together
#include "stm32f100xx_gpio.h"
#include "stm32f100xx_spi.h"
#include "stm32f100xx_i2c.h"
#include "stm32f100xx_usart.h"
#include "stm32f100xx_rcc.h"


#endif /* INC_STM32F1XX_H_ */
