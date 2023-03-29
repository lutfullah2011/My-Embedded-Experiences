/*
 * lcd.h
 *
 *  Created on: 20 Mar 2023
 *      Author: lutfullah.aydin
 */

#ifndef LCD_H_
#define LCD_H_


#include "stm32f1xx.h"


/*Application configurable items */
#define LCD_GPIO_PORT  GPIOC
#define LCD_GPIO_RS	   GPIO_Pin_0
#define LCD_GPIO_RW	   GPIO_Pin_1
#define LCD_GPIO_EN	   GPIO_Pin_2
#define LCD_GPIO_D4	   GPIO_Pin_6
#define LCD_GPIO_D5	   GPIO_Pin_7
#define LCD_GPIO_D6	   GPIO_Pin_8
#define LCD_GPIO_D7	   GPIO_Pin_9


/*LCD commands according to the Datasheet */
#define LCD_CMD_4DL_2N_5X8F  		0x28 //4 Data length, 2 lines, 5x8 fonts
#define LCD_CMD_DON_CURON    		0x0E //LCD Display ON and Curser ON
#define LCD_CMD_INCADD       		0x06 //Display increment RAM address
#define LCD_CMD_CLEAR    			0X01 //LCD Clear Display
#define LCD_CMD_RETURN_HOME  		0x02 //Return the default place, home.


/******************************************************************************/
/*                  Function Prototype by this driver - APIs	              */
/******************************************************************************/

void lcd_init(void);
void lcd_send_command(uint8_t cmd);
void lcd_print_char(uint8_t data);
void lcd_clear(void);
void lcd_return_home(void);
void lcd_print_string(char*);
void lcd_set_cursor(uint8_t row, uint8_t column);



#endif /* LCD_H_ */
