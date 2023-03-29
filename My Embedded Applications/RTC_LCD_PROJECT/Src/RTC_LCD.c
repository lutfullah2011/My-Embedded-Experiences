/*
 * RTC_LCD.c
 *
 *  Created on: 20 Mar 2023
 *      Author: lutfullah.aydin
 */


/***********************************************************************************************/
/*             			    	 EXPLANATION OF THIS PROJECT					    		   */
/***********************************************************************************************/

/*
 * This application shows the basic usage of an RTC(Real Time Clock) and its LCD demonstration.
 * After setting the time and date infos, we can get the time and date infos in real time world.
 * This application runs 1 times in main and continues with the systick timer interrrupts(every seconds)
 *
 *
 ****************************APPLICATION LAYER EXPLANATION**************************************************
 * In this project, First, I use my own specific drivers(GPIO & I2C) as MCU Peripheral layer.
 * In this project, I implement and use drivers for BSP(Board Support Package) which are "lcd.c","lcd.h","ds1307.c","ds1307.h". This Layer abstract the usage of RTC and LCD.
 * In this project, Lastly, this file is the application layer(RTC_LCD.c)
 *
 *
 * In this prooject, while debugging mode, it occurs "hard fault" and goes to RESET HANDLER. It because, program hangs in while loop, thats why!!
 *
 */


#include<stdio.h>
#include "ds1307.h"
#include "lcd.h"

#define SYSTICK_TIM_CLK   8000000UL //8MH Clock frequency (Nothing but a HSI speed)

// We can use this macro if we want to use LCD print or we could use "printf" with Semihosting
#define PRINT_LCD

//Function Prototypes
void init_systick_timer(uint32_t tick_hz);
char* get_day_of_week(uint8_t i);
void number_to_string(uint8_t num , char* buf);
char* time_to_string(RTC_time_t *rtc_time);
char* date_to_string(RTC_date_t *rtc_date);
void delay(uint32_t time);



int main(void)
{

	RTC_time_t current_time;
	RTC_date_t current_date;

#ifndef PRINT_LCD
	printf("Real Time Clock\n");
#else
	lcd_init();

	lcd_print_string("Real Time Clock");

	delay(10000);

	lcd_clear();
	lcd_return_home();
#endif

	if(ds1307_init()){
		printf("RTC Init failed\n");
		while(1);
	}

	init_systick_timer(1);

	current_date.day = FRIDAY;
	current_date.date = 20;
	current_date.month = 3;
	current_date.year = 23;

	current_time.hours = 10;
	current_time.minutes = 39;
	current_time.seconds = 10;
	current_time.time_format = TIME_FORMAT_12HRS_PM;

	ds1307_set_current_date(&current_date);
	ds1307_set_current_time(&current_time);

	ds1307_get_current_time(&current_time);
	ds1307_get_current_date(&current_date);

	char *am_pm;
	if(current_time.time_format != TIME_FORMAT_24HRS){
		am_pm = (current_time.time_format) ? "PM" : "AM";
#ifndef PRINT_LCD
		printf("Current time = %s %s\n",time_to_string(&current_time),am_pm); // 10:39:10 PM
#else
		lcd_print_string(time_to_string(&current_time));
		lcd_print_string(am_pm);
#endif
	}else{
#ifndef PRINT_LCD
		printf("Current time = %s\n",time_to_string(&current_time)); // 10:39:10
#else
		lcd_print_string(time_to_string(&current_time));
#endif
	}

#ifndef PRINT_LCD
	printf("Current date = %s <%s>\n",date_to_string(&current_date), get_day_of_week(current_date.day));
#else
	lcd_set_cursor(2, 1);
	lcd_print_string(date_to_string(&current_date));
#endif


	while(1);

	return 0;
}






/*
 * This systick timer creates interrrupt in every seconds.
 * "tick_hz" variable should be 1 in order the get default system clock value
 * Systick timer is a downcounter and when it down to 0, it creates interrupt
 * This information can be found in ARM_Cortex_M3 User Guide
 */
void init_systick_timer(uint32_t tick_hz)
{
	uint32_t *pSCSR = (uint32_t*)0xE000E010; //SysTick Control and Status Register Address Pointer
	uint32_t *pSRVR = (uint32_t*)0xE000E014; //SysTick Reload Value Register Address Pointer

    //Calculation of downcounter value, Since we want interrupt in every seconds, we arrange the value of SYSTICK_TIM_CLK/1
    uint32_t count_value = (SYSTICK_TIM_CLK/tick_hz)-1;

    //Clear the value of SRVR
    *pSRVR &= ~(0x00FFFFFFFF);

    //load the value in to SRVR
    *pSRVR |= count_value;

    //do some settings(Indicated in ARM User Guide page 160)
    *pSCSR |= ( 1 << 1); //Enables SysTick exception request
    *pSCSR |= ( 1 << 2); //Indicates the clock source, we choose processor clock source

    //Enable the systick
    *pSCSR |= ( 1 << 0); //Enables the counter loads the RELOAD value from SRVR and counts down.

}


char* get_day_of_week(uint8_t i)
{
	char* days[] = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

	return days[i-1];
}


void number_to_string(uint8_t num , char* buf)
{

	if(num < 10){ // If number smalller then 10, show it like "06".
		buf[0] = '0';
		buf[1] = num+48; //Converting the number into ASCII format for becoming string
	}else if(num >= 10 && num < 99)
	{
		buf[0] = (num/10) + 48;
		buf[1]= (num % 10) + 48;
	}
}



//Showing the time is the format of "hh:mm:ss"
char* time_to_string(RTC_time_t *rtc_time)
{
	static char buf[9];

	buf[2]= ':';
	buf[5]= ':';

	number_to_string(rtc_time->hours,buf);
	number_to_string(rtc_time->minutes,&buf[3]);
	number_to_string(rtc_time->seconds,&buf[6]);

	buf[8] = '\0';

	return buf;

}

//Showing the date is the format of "dd/mm/yy"
char* date_to_string(RTC_date_t *rtc_date)
{
	static char buf[9];

	buf[2]= '/';
	buf[5]= '/';

	number_to_string(rtc_date->date,buf);
	number_to_string(rtc_date->month,&buf[3]);
	number_to_string(rtc_date->year,&buf[6]);

	buf[8]= '\0';

	return buf;

}

void delay(uint32_t time){

	while(time--);

}


/*
 * This handler is fired when every second(according to this project value)
 * the systick timer is initiated.
 */
void SysTick_Handler(void)
{
	RTC_time_t current_time;
	RTC_date_t current_date;

	ds1307_get_current_time(&current_time);

	char *am_pm;
	if(current_time.time_format != TIME_FORMAT_24HRS){
		am_pm = (current_time.time_format) ? "PM" : "AM";
#ifndef PRINT_LCD
		printf("Current time = %s %s\n",time_to_string(&current_time),am_pm); // 10:39:10 PM
#else
		lcd_set_cursor(1, 1);
		lcd_print_string(time_to_string(&current_time));
		lcd_print_string(am_pm);
#endif

	}else{
#ifndef PRINT_LCD
		printf("Current time = %s\n",time_to_string(&current_time)); // 10:39:10
#else
		lcd_set_cursor(1, 1);
		lcd_print_string(time_to_string(&current_time));
#endif
	}

	ds1307_get_current_date(&current_date);

#ifndef PRINT_LCD
	printf("Current date = %s <%s>\n",date_to_string(&current_date), get_day_of_week(current_date.day));
#else
	lcd_set_cursor(2, 1);
	lcd_print_string(date_to_string(&current_date));
	lcd_print_char('<');
	lcd_print_string(get_day_of_week(current_date.day));
	lcd_print_char('>');
#endif


}
