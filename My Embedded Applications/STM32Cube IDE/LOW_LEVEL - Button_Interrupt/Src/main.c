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

//NVIC adresini bulamadım veya yanlış yerdeyim. Proje yarım kaldı

#include <stdint.h>
#include <stdio.h>

volatile uint8_t button_press = 0; //Volatile çünkü interruptda kullanıcam.
uint32_t button_counter = 0;

void button_init(void);
void gpiox_init(void);

volatile uint32_t *pCLKregAPB2			 = (uint32_t*)0x40021018; //PORT C Clock adress for both led and button = RCC_APB2ENR
volatile uint32_t *pPortCModeRegH 		 = (uint32_t*)0x40011004; //PORT C Mode adress leds
volatile uint32_t *pPortCModeRegL 		 = (uint32_t*)0x40011000; //PORT C Mode adress button
volatile uint32_t *pPortCODRReg 		=  (uint32_t*)0x4001100C; //PORT C ODR  adress
volatile uint32_t *pEXTIMaskReg          = (uint32_t*) (0x40010400 + 0x00);
volatile uint32_t *pEXTTIEdgeCtrlReg      = (uint32_t*) (0x40010400 + 0x08);
volatile uint32_t *pEXTTIPendReg         = (uint32_t*) (0x40010400 + 0x14);
//volatile uint32_t *pNVICIRQEnReg          = (uint32_t*) 0xE000E100;
//EXTI0 EXTI Line0 interrupt 0x0000_0058

int main(void)
{
	button_init();

	while (1) {
		//Disable interrupt
		*pEXTIMaskReg &= ~(1 << 0);

		if (button_press) {
			//Some delay until button debouncing gets over
			for (uint32_t volatile i = 0; i < 500000 / 2; i++)
				;
			button_counter++;
			printf("Button is pressed : %lu\n", button_counter);
			button_press = 0;
		}

		//Enable interrupt
		*pEXTIMaskReg |= (1 << 0);
	}



}


void gpiox_init(void)
{

	//1. enable the clock for GPOIC peripheral in the APB2ENR (SET the 4th bit position)
	*pCLKregAPB2 |= ( 1 << 4);
	//enable the clock for AF EXTI peripheral in the APB2ENR (SET the 0th bit position)
	*pCLKregAPB2 |= ( 1 << 0);

	//2. configure the mode of the IO pin as output and speed // In memory map, default is 0100
	//   make 0th bit position as 1 (SET)

	*pPortCModeRegH |= ( 819 << 0);// (GPIOx_CRH) Pin 8-9-10  output mode,max speed 50, push pull
	//0011 0011 0011


}


void button_init(void)
{
	*pCLKregAPB2 |= (1 << 14); //Button :PORT C0

	*pPortCModeRegL |= ( 3 << 0);// (GPIOx_CRL) Pin 0
	*pPortCModeRegL &= ~( 3 << 2);

	*pEXTTIEdgeCtrlReg |= (1 << 0);

	*pEXTIMaskReg |= (1 << 0);

	//*pNVICIRQEnReg |= (1 << 6);

}

/* This is button interrupt handler*/
void EXTI0_IRQHandler(void)
{
	//Make this flag SET . if button pressed
	button_press = 1;

	for(int i=0; i<4; i++){
	//SET the bits of the output data register to make I/O pin-8-9-10 as HIGH
	*pPortCODRReg |= ( 7 << 8);
	for (uint32_t volatile i = 0; i < 1000000; i++)
	*pPortCODRReg &= ~( 7 << 8);
	}

	*pEXTTIPendReg |= (1 << 0);
}
