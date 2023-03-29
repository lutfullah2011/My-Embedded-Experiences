/*
 * ds1307.h
 *
 *  Created on: 20 Mar 2023
 *      Author: lutfullah.aydin
 */

#ifndef DS1307_H_
#define DS1307_H_


#include "stm32f1xx.h"

/*Device Slave adress */
#define DS1307_I2C_ADDRESS      0x68


/*Application configurable items */
#define DS1307_I2C  			I2C1
#define DS1307_I2C_GPIO_PORT    GPIOB
#define DS1307_I2C_SCL_PIN 		GPIO_Pin_6
#define DS1307_I2C_SDA_PIN 		GPIO_Pin_7
#define DS1307_I2C_SPEED 		I2C_SCL_SPEED_2K


/*Register addresses */
#define DS1307_REGADDR_SEC 		0x00
#define DS1307_REGADDR_MIN 		0x01
#define DS1307_REGADDR_HRS		0x02
#define DS1307_REGADDR_DAY		0x03
#define DS1307_REGADDR_DATE		0x04
#define DS1307_REGADDR_MONTH	0x05
#define DS1307_REGADDR_YEAR		0x06


/*
 * Time format specifications
 */
#define TIME_FORMAT_12HRS_AM 	0
#define TIME_FORMAT_12HRS_PM 	1
#define TIME_FORMAT_24HRS 		2


/*
 * Days of the week Macros
 */
#define SUNDAY  	1;
#define MONDAY  	2;
#define TUESDAY  	3;
#define WEDNESDAY   4;
#define THURSDAY  	5;
#define FRIDAY  	6;
#define SATURDAY  	7;


/*
 * Structure for date
 */
typedef struct
{
	uint8_t date; 		/* Date in a month, 1 to 31 */
	uint8_t month;		/* Month in a year, 1 to 12 */
	uint8_t year; 		/* Year parameter, 00 to 99, 00 is 2000 and 99 is 2099 */
	uint8_t day; 		/* Day in a week, from 1 to 7 */
}RTC_date_t;

/*
 * Structure for time
 */
typedef struct
{
	uint8_t seconds; 		/* Seconds parameter, from 00 to 59 */
	uint8_t minutes;		/* Minutes parameter, from 00 to 59 */
	uint8_t hours;			/* Hours parameter, 24Hour mode, 00 to 23 */
	uint8_t time_format;	/* Time format parameter, 12HRS_AM, 12HRS_PM, 24HRS */
}RTC_time_t;




/******************************************************************************/
/*                  Function Prototype by this driver - APIs	              */
/******************************************************************************/

uint8_t ds1307_init(void);

void ds1307_set_current_time(RTC_time_t *);
void ds1307_get_current_time(RTC_time_t *);

void ds1307_set_current_date(RTC_date_t *);
void ds1307_get_current_date(RTC_date_t *);





#endif /* DS1307_H_ */
