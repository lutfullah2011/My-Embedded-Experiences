/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */


#include <stdint.h>
#include <stdio.h>


uint32_t button_counter = 0;

void button_init(void);
void gpiox_init(void);
void delay(uint32_t time);

volatile uint32_t *pCLKregAPB2			 = (uint32_t*)0x40021018; //PORT C Clock & AFIO Enable adress = RCC_APB2ENR
volatile uint32_t *pPortCModeRegL 		 = (uint32_t*)0x40011000; //PORT C Mode adress for button config
volatile uint32_t *pPortCModeRegH 		 = (uint32_t*)0x40011004; //PORT C Mode adress for led config
volatile uint32_t *pPortCODRReg 		 = (uint32_t*)0x4001100C; //PORT C ODR  adress
volatile uint32_t *pAFIOEXTICR1Reg        = (uint32_t*)(0x40010000 + 0x08); // AFIO EXTICR address enabling for Port source
volatile uint32_t *pEXTIMaskReg          = (uint32_t*)(0x40010400 + 0x00);
volatile uint32_t *pEXTTIEdgeCtrlReg     = (uint32_t*)(0x40010400 + 0x08); // Rising Trigger adress.
volatile uint32_t *pEXTTIPendReg         = (uint32_t*)(0x40010400 + 0x14);
volatile uint32_t *pNVICIRQEnReg         = (uint32_t*)0xE000E100; // The adress is in the Cortex-M3 Generic User Guide pdf.

int main(void)
{
	button_init();
	gpiox_init();

	while (1) {

		//SET the bits of the output data register to make I/O pin-8-9-10 as HIGH
		*pPortCODRReg |= ( 7 << 8);

	}



}


void delay(uint32_t time){

	while(time--);

}


void gpiox_init(void)
{

	//1. enable the clock for GPOIC peripheral in the APB2ENR (SET the 4th bit position)
	*pCLKregAPB2 |= ( 1 << 4);

	//2. configure the mode of the IO pin as output and speed // In memory map, default is 0100
	//   make 0th bit position as 1 (SET)

	*pPortCModeRegH |=  ( 819 << 0);// (GPIOx_CRH) Pin 8-9-10  output mode,max speed 50, push pull | 0011 0011 0011 = 819 as decimal
	*pPortCModeRegH &= ~( 3276 << 0); // We need to clear CNF bits of CRH Registers. ~(1100 1100 1100) = 3276 as decimal

}


void button_init(void)
{
	*pCLKregAPB2 |= (1 << 0); //Button :PORT C0 enabled while setting led config
							 // AFIO Clock enable for interrup(EXTI Peripheral) config

	//(GPIOx_CRL) Pin 0 as input mode, actually reset state is input mode, no need setting for this pin actually

	*pEXTTIEdgeCtrlReg |= (1 << 0); // pin0 edge detection config

	*pEXTIMaskReg |= (1 << 0); // Enable interrupt for pin0

	*pNVICIRQEnReg |= (1 << 6); // EXTI0 IRQ no is 6

	*pAFIOEXTICR1Reg |= (1 << 1); // Port C source code is 2 in EXTICR1 Reg

}

/* This is button interrupt handler*/
void EXTI0_IRQHandler(void)
{
	button_counter++; // For debugging concern

	for(int i=0; i<4; i++){
	//SET the bits of the output data register to make I/O pin-8-9-10 as HIGH
		*pPortCODRReg ^= ( 7 << 8);
		delay(500000);
	}

	*pEXTTIPendReg |= (1 << 0); //Clear the pending bit
}

