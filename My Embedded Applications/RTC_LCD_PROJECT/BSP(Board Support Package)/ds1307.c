/*
 * ds1307.c
 *
 *  Created on: 20 Mar 2023
 *      Author: lutfullah.aydin
 */


#include <stdint.h>
#include <string.h>

#include "ds1307.h"


//Specific function prototypes to this files
static void ds1307_GPIOInit(void);
static void ds1307_I2CInit(void);
static uint8_t ds1307_i2c_read(uint8_t reg_addr);
static void ds1307_i2c_write(uint8_t value,uint8_t reg_addr);
static uint8_t bcd2bin(uint8_t value);
static uint8_t bin2bcd(uint8_t value);


I2C_Handle_t ds1307i2cHandle; // Define this handle as Global variable because we will use it:)

/*
 * This API is simply initiate the RTC and
 * returns 1 : CH(Clock Halt) = 1 ; init failed
 * returns 0 : CH(Clock Halt) = 0 ; init success
 */
uint8_t ds1307_init(void)
{

	//Initialization of the GPIO's I2C pins
	ds1307_GPIOInit();

	//Initialization of the I2C Peripheral
	ds1307_I2CInit();

	//Enable the I2C peripheral
	I2C_PeriphControl(DS1307_I2C, ENABLE);

	//Make Clock halt = 0 to start clocking according to the datasheet
	ds1307_i2c_write(0x00,DS1307_REGADDR_SEC);//write this register 0 to start running

	//Read clock halt bit again to take clock state
	uint8_t clock_state = ds1307_i2c_read(DS1307_REGADDR_SEC);

	return ((clock_state >> 7 ) & 0x1); // 7th bit is the CH bit of the rtc device reg.

}


void ds1307_set_current_time(RTC_time_t *rtc_time)
{
	uint8_t seconds, hrs;
	seconds = bin2bcd(rtc_time->seconds);
	seconds &= ~( 1 << 7); //7th bit is the clock hal bit and must be kept at 0. thats why we do this line. Clearing the 7th bit.
	ds1307_i2c_write(seconds, DS1307_REGADDR_SEC);
	ds1307_i2c_write(bin2bcd(rtc_time->minutes), DS1307_REGADDR_MIN);

	hrs = bin2bcd(rtc_time->hours);

	if(rtc_time->time_format == TIME_FORMAT_24HRS){
		hrs &= ~(1 << 6);//6th bit is cleared according to datasheet
	}else
	{
		hrs |= (1 << 6);//6th bit is set
		hrs = (rtc_time->time_format  == TIME_FORMAT_12HRS_PM) ? hrs | ( 1 << 5) :  hrs & ~( 1 << 5) ;//5th bit is set when PM format, 5th bit is cleared when AM format
	}

	ds1307_i2c_write(hrs,DS1307_REGADDR_HRS);

}

void ds1307_set_current_date(RTC_date_t *rtc_date)
{
	ds1307_i2c_write(bin2bcd(rtc_date->date),DS1307_REGADDR_DATE);

	ds1307_i2c_write(bin2bcd(rtc_date->month),DS1307_REGADDR_MONTH);

	ds1307_i2c_write(bin2bcd(rtc_date->year),DS1307_REGADDR_YEAR);

	ds1307_i2c_write(bin2bcd(rtc_date->day),DS1307_REGADDR_DAY);

}

void ds1307_get_current_time(RTC_time_t *rtc_time)
{

	uint8_t seconds,hrs;

	seconds = ds1307_i2c_read(DS1307_REGADDR_SEC);

	seconds &= ~( 1 << 7); //Bit number 7 is not relevant with desired data. It is Clock halt bit.

	rtc_time->seconds = bcd2bin(seconds); //Converting bcd data to binary format
	rtc_time->minutes = bcd2bin(ds1307_i2c_read(DS1307_REGADDR_MIN));

	hrs = ds1307_i2c_read(DS1307_REGADDR_HRS);

	if(hrs & ( 1 << 6)){ //If its set, it is 12 hour format
		rtc_time->time_format =  !((hrs & ( 1 << 5)) == 0) ; //If this statement is true(5th bit is 0), It is AM format, otherwise PM.
		hrs &= ~(0x3 << 5);//Clear the 6th and 5th bit
	}else{
		//24 hour format
		rtc_time->time_format = TIME_FORMAT_24HRS;
	}

	rtc_time->hours = bcd2bin(hrs);
}

void ds1307_get_current_date(RTC_date_t *rtc_date)
{
	rtc_date->day   = bcd2bin(ds1307_i2c_read(DS1307_REGADDR_DAY));
	rtc_date->date  = bcd2bin(ds1307_i2c_read(DS1307_REGADDR_DATE));
	rtc_date->month = bcd2bin(ds1307_i2c_read(DS1307_REGADDR_MONTH));
	rtc_date->year  = bcd2bin(ds1307_i2c_read(DS1307_REGADDR_YEAR));
}


static void ds1307_GPIOInit(void)
{
	GPIO_Handle_t i2c_sda,i2c_scl;

	/*
	 * PA6 --> I2C1_SCL - Alternate Function Open Drain
	 * PA7 --> I2C1_SDA - Alternate Function Open Drain
	 */

	i2c_sda.pGPIOx = DS1307_I2C_GPIO_PORT;
	i2c_sda.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_AF_OD;
	i2c_sda.GPIO_PinConfig.GPIO_Pin = DS1307_I2C_SDA_PIN;
	i2c_sda.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_HIGH;
	GPIO_Init(&i2c_sda);


	i2c_scl.pGPIOx = DS1307_I2C_GPIO_PORT;
	i2c_scl.GPIO_PinConfig.GPIO_Mode = GPIO_MODE_AF_OD;
	i2c_scl.GPIO_PinConfig.GPIO_Pin  = DS1307_I2C_SCL_PIN;
	i2c_scl.GPIO_PinConfig.GPIO_Speed = GPIO_SPEED_HIGH;
	GPIO_Init(&i2c_scl);

}


static void ds1307_I2CInit(void)
{
	ds1307i2cHandle.pI2Cx = DS1307_I2C;
	ds1307i2cHandle.I2CConfig.I2C_ACK = I2C_ACK_ENABLE;
	ds1307i2cHandle.I2CConfig.I2C_SCLSpeed = DS1307_I2C_SPEED;
	I2C_Init(&ds1307i2cHandle);
}


static void ds1307_i2c_write(uint8_t value,uint8_t reg_addr)
{
	uint8_t tx[2];
	tx[0] = reg_addr;
	tx[1] = value;
	I2C_MasterSendData(&ds1307i2cHandle, tx, 2, DS1307_I2C_ADDRESS, 0);
}



static uint8_t ds1307_i2c_read(uint8_t reg_addr)
{
	uint8_t data;
    I2C_MasterSendData(&ds1307i2cHandle, &reg_addr, 1, DS1307_I2C_ADDRESS, 0);
    I2C_MasterReceiveData(&ds1307i2cHandle, &data, 1, DS1307_I2C_ADDRESS, 0);

    return data;
}


/*
 * Binary to BCD(Binary coded decimal) conversion
 * We need to use BCD format with the Device registers.
 */
static uint8_t bin2bcd(uint8_t value)
{
	uint8_t bcd;

	bcd = ((value /10) << 4) | (value % 10) ;

	return bcd;
}

/*
 * BCD(Binary coded decimal) to Binary conversion
 */
static uint8_t bcd2bin(uint8_t value)
{
	uint8_t dec;
	dec = (uint8_t) ((value >> 4 ) * 10);
	dec +=  value & (uint8_t)0x0F;
	return dec;
}



