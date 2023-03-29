/*
 * lcd.c
 *
 *  Created on: 20 Mar 2023
 *      Author: lutfullah.aydin
 */



#include "lcd.h"

static void write_4_bits(uint8_t value);
static void lcd_enable(void);
static void mdelay(uint32_t cnt);
static void udelay(uint32_t cnt);


void lcd_send_command(uint8_t cmd)
{
	/* RS=0 for LCD command */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);

	/*R/W = 0, for write */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	//8 bit transmitted like this, first Higher 4 bits, then lower 4 bits.
	write_4_bits(cmd >> 4);
	write_4_bits(cmd & 0x0F);

}

/*
 *This function sends a "character" to the LCD
 *Since we used 4 bit parallel data transmission, datas transmitted according to:
 *			First higher 4 bit of the data will be sent on to the data lines D4,D5,D6,D7
 *			Then lower 4 bit of the data will be sent on to the data lines D4,D5,D6,D7
 */
void lcd_print_char(uint8_t data)
{
	/* RS=1 for LCD user data */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_SET);

	/*R/nW = 0, for write */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	write_4_bits(data >> 4);  /*Higher 4 bit*/
	write_4_bits(data & 0x0F); /*Lower 4 bit*/

}

/*
 * This function sends a "string" to the LCD
 * Same logic with the character transmission
 */
void lcd_print_string(char *message)
{

      do
      {
          lcd_print_char((uint8_t)*message++);
      }
      while (*message != '\0'); //Send until the "NULL" character.

}


/*
 * LCD initialization according to the datasheet
 * First part include GPIO Init
 * Second part include LCD Init step by step according to the datasheet.
 */
void lcd_init(void)
{

	//Configuration of the GPIO pins of LCD
	GPIO_Handle_t lcd_pins;

	lcd_pins.pGPIOx = LCD_GPIO_PORT;
	lcd_pins.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_OUT_PP;
	lcd_pins.GPIO_PinConfig.GPIO_Pin = LCD_GPIO_RS;
	lcd_pins.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_HIGH;
	GPIO_Init(&lcd_pins);

	lcd_pins.GPIO_PinConfig.GPIO_Pin = LCD_GPIO_RW;
	GPIO_Init(&lcd_pins);

	lcd_pins.GPIO_PinConfig.GPIO_Pin = LCD_GPIO_EN;
	GPIO_Init(&lcd_pins);

	lcd_pins.GPIO_PinConfig.GPIO_Pin = LCD_GPIO_D4;
	GPIO_Init(&lcd_pins);

	lcd_pins.GPIO_PinConfig.GPIO_Pin = LCD_GPIO_D5;
	GPIO_Init(&lcd_pins);

	lcd_pins.GPIO_PinConfig.GPIO_Pin = LCD_GPIO_D6;
	GPIO_Init(&lcd_pins);

	lcd_pins.GPIO_PinConfig.GPIO_Pin = LCD_GPIO_D7;
	GPIO_Init(&lcd_pins);

	//Reset all lcd pins initially (I am not sure that we need this or not)
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, GPIO_PIN_RESET);

/******************************************************************************************************/

	//LCD initialization according to the datasheet(Every steps explained in datasheet)

	//1. After Power on, we need to wait at least 40 ms
	mdelay(40);

	//Setting LCD for writing Instructor register(RS=0, RW=0) according to the datasheet.
	/*RS = 0 , For LCD command */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);
	/* R/W = 0, Writing to LCD */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	//2. Writing the command for DB5 and DB4 (0x3), according to the datasheet.
	write_4_bits(0x3);

	//3. wait more than 4.1 miliseconds
	mdelay(5);

	//4. Writing again the command for DB5 and DB4 (0x3), according to the datasheet.
	write_4_bits(0x3);

	//5. wait more than 100 microseconds
	udelay(150);

	//6. First ,writing again the command for DB5 and DB4 (0x3), then writing the command for DB5(0x2) according to the datasheet.
	write_4_bits(0x3);
	write_4_bits(0x2);

	//7. Set for function command, 4 bit data, character font etc.
	lcd_send_command(LCD_CMD_4DL_2N_5X8F);

	//8. Display and cursor ON
	lcd_send_command(LCD_CMD_DON_CURON);

	//9. Display clear
	lcd_clear();

	//10. Set for entry mode, Increment RAM address.
	lcd_send_command(LCD_CMD_INCADD);



}


/*
 * Since we used 4 bit parallel data transmission, this function writes 4 bits of data/command on to D4,D5,D6,D7 lines
 * According to the datasheet you must not use D0,D1,D2,D3.
 */
static void write_4_bits(uint8_t value)
{
	GPIO_WriteToOutputPin(LCD_GPIO_PORT,LCD_GPIO_D4, ((value >> 0) & 0x1) ); //Bring the bit to the position of LSB position and mask other bits.
	GPIO_WriteToOutputPin(LCD_GPIO_PORT,LCD_GPIO_D5, ((value >> 1) & 0x1) );
	GPIO_WriteToOutputPin(LCD_GPIO_PORT,LCD_GPIO_D6, ((value >> 2) & 0x1) );
	GPIO_WriteToOutputPin(LCD_GPIO_PORT,LCD_GPIO_D7, ((value >> 3) & 0x1) );

	lcd_enable();
}


void lcd_clear(void)
{
	//Display clear
	lcd_send_command(LCD_CMD_CLEAR);

	// display clear command execution wait time is around 2ms according to the datasheet
	mdelay(2);
}


/*Cursor returns to home position */
void lcd_return_home(void)
{
	lcd_send_command(LCD_CMD_RETURN_HOME);

	// return home command execution wait time is around 2ms according to the datasheet
	mdelay(2);
}


/*
 *   This function helps to set Lcd to a specified location given by row and column information
 *   Row Number (1 to 2)
 *   Column Number (1 to 16) Assuming a 2 X 16 characters display
 *   We set the DDRAM adresses to configure specific adresses according to the datasheet.
 */
void lcd_set_cursor(uint8_t row, uint8_t column)
{
  column--; //(Initially decremented since the values between 1-16 become ==> 0-15 )
  switch (row)
  {
    case 1:
      /* Set cursor to 1st row address and add index*/
      lcd_send_command((column |= 0x80)); // When setting DDRAM adress, we have to set DB7 as 1 (1000 0000) and the adress for first row is 00 according to the datasheet
      break;
    case 2:
      /* Set cursor to 2nd row address and add index*/
      lcd_send_command((column |= 0xC0)); // When setting DDRAM adress, we have to set DB7 as 1 (1010 0000) and the adress for second row is 40 according to the datasheet
      break;
    default:
      break;
  }
}


/*
 * For LCD enable, we need to do High to Low transition. That time, LCD reads the data from data lines.
 */
static void lcd_enable(void)
{
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_SET);
	udelay(100);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);
	udelay(300);/* execution time > 37 micro seconds */
}


static void mdelay(uint32_t cnt)
{
	for(uint32_t i=0 ; i < (cnt * 1000); i++);
}

static void udelay(uint32_t cnt)
{
	for(uint32_t i=0 ; i < (cnt * 1); i++);
}
