/*
 * stm32f100xx_i2c.h
 *
 *  Created on: 12 Oca 2023
 *      Author: lutfullah_aydin
 */

#ifndef INC_STM32F100XX_I2C_H_
#define INC_STM32F100XX_I2C_H_


#include "stm32f1xx.h"


/******************************************************************************/
/*                  Structures for I2Cx Peripheral			    	          */
/******************************************************************************/

/*
 * Configuration Structure for I2Cx Peripheral
 */
typedef struct
{
  uint32_t I2C_SCLSpeed;       		/* Specifies the I2C Serial Clock Speed		*/
  uint8_t  I2C_DeviceAddress;       /* Specifies the I2C Device adress(7 Bit) 			*/
  uint8_t  I2C_ACK;       	/* Specifies the I2C ACK bit Control, ENABLE or DISABLE		*/
  uint8_t  I2C_FastModeDutyCycle;   /* Specifies the I2C Fast MODE Duty Cycle 	*/

}I2C_Config_t;



/*
 * Handle Structure for I2Cx Peripheral
 */
typedef struct
{
	I2C_RegDef_t 	*pI2Cx;				/*This holds the base address of I2Cx Peripheral	*/
	I2C_Config_t 	I2CConfig;			/*This holds I2Cx Peripheral configuration settings*/

	uint8_t 		*pTxBuffer; 		/*  To store the app. Tx buffer address  */
	uint8_t 		*pRxBuffer;			/*  To store the app. Rx buffer address  */
	uint32_t 		TxLen;				/*  To store Tx len  */
	uint32_t 		RxLen;				/*  To store Rx len  */
	uint8_t 		TxRxState;			/*  To store Communication state. Since I2C has Half-duplex, only 1 variable is enough to implement  */
	uint8_t 		DevAddr;			/*  To store Device Address  */
	uint32_t 		RxSize;				/*  To store Rx Size. RxSize is used in ISR to manage data reception  */
	uint8_t 		Sr;					/*  To store Repeated Start value  */

}I2C_Handle_t;




/*
 * I2C application states
 */
#define I2C_READY 					0
#define I2C_BUSY_RX 				1
#define I2C_BUSY_TX 				2



/*
 * @I2C_SCLSpeed
 */
#define I2C_SCL_SPEED_STD		100000  //100KHz = Standart Mode
#define I2C_SCL_SPEED_FAST		400000  //400KHz = Fast Mode


/*
 * @I2C_ACK
 */
#define I2C_ACK_ENABLE        1
#define I2C_ACK_DISABLE       0 //Default


/*
 * @I2C_FastModeDutyCycle
 */
#define I2C_Fast_Mode_DUTY_2        0 // Look at the Reference Manual
#define I2C_Fast_Mode_DUTY_16_9     1



/*
 * I2C Status Flag Definitions
 */
#define I2C_FLAG_TXE   		( 1 << I2C_SR1_TXE)
#define I2C_FLAG_RXNE   	( 1 << I2C_SR1_RXNE)
#define I2C_FLAG_SB			( 1 << I2C_SR1_SB)
#define I2C_FLAG_OVR  		( 1 << I2C_SR1_OVR)
#define I2C_FLAG_AF   		( 1 << I2C_SR1_AF)
#define I2C_FLAG_ARLO 		( 1 << I2C_SR1_ARLO)
#define I2C_FLAG_BERR 		( 1 << I2C_SR1_BERR)
#define I2C_FLAG_STOPF 		( 1 << I2C_SR1_STOPF)
//#define I2C_FLAG_ADD10 		( 1 << I2C_SR1_ADD10)
#define I2C_FLAG_BTF  		( 1 << I2C_SR1_BTF) //Byte transfer finished can be used to prevent underrun in TX
#define I2C_FLAG_ADDR 		( 1 << I2C_SR1_ADDR)
#define I2C_FLAG_TIMEOUT 	( 1 << I2C_SR1_TIMEOUT)

#define I2C_DISABLE_SR  	RESET
#define I2C_ENABLE_SR   	SET


/*
 * I2C application events & errors macros
 */
#define I2C_EV_TX_CMPLT  	 	0 //Master mode Transmission complete event
#define I2C_EV_RX_CMPLT  	 	1 //Master mode Reception complete event
#define I2C_EV_STOPF       		2 //Stop generated Flag. When slave detects STOP (setting STOPF), master ended the WRITE transaction. Also, STOPF flag sets only in slave receiver mode, NOT in transmitter mode
#define I2C_ERROR_BERR 	 		3 //Bus error happens when SDA rising or falling non-valid
#define I2C_ERROR_ARLO  		4 //Arbitration loss error happens when arbitration of the bus to another master
#define I2C_ERROR_AF    		5 //Ack failure error
#define I2C_ERROR_OVR   		6 //Overrun error happens always during reception. New byte arrived but DR is not ready, so we lost the data. However, in I2C it does not occur while clock streching enable.
#define I2C_ERROR_TIMEOUT 		7 //Happens when holding clock more than recomended
#define I2C_EV_DATA_REQ         8 //Slave mode data request event
#define I2C_EV_DATA_RCV         9 //Slave mode data receive event
//Underrun error happens always during transmisson
//PEC error happens when CRC mismatch

/******************************************************************************/
/*                  Function Prototype by this driver - APIs	              */
/******************************************************************************/

/*
 * Peripheral Clock Setup
 */
void I2C_PCLK(I2C_RegDef_t *pI2Cx, uint8_t EnorDis);/*Peripheral Clock Setup(Enable or Disable)*/

/*
 * SPIx Peripheral Init and De-Init
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);	/*Peripheral Initialization*/
void I2C_DeInit(I2C_RegDef_t *pI2Cx);		/*Peripheral Deinitialization*/

/*
 * Data Send and Receive
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle,uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr);
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr,uint8_t Sr);
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr);
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr,uint8_t Sr);

void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle);
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);


void I2C_SlaveSendData(I2C_RegDef_t *pI2C,uint8_t data);
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2C);

/*
 * IRQ Configuration and ISR handling
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);


/*
 * Other Peripheral Control APIs
 */
void I2C_PeriphControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx , uint32_t FlagName);
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnorDis);
void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);

void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx,uint8_t EnorDis);

/*
 * Application callback
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv);




#endif /* INC_STM32F100XX_I2C_H_ */
