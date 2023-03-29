/*
 * stm32f100xx_usart.h
 *
 *  Created on: 7 Mar 2023
 *      Author: lutfullah_aydin
 */

#ifndef INC_STM32F100XX_USART_H_
#define INC_STM32F100XX_USART_H_


#include "stm32f1xx.h"


/******************************************************************************/
/*                  Structures for USARTx Peripheral			    	          */
/******************************************************************************/


/*
 * Configuration structure for USARTx peripheral
 */
typedef struct
{
	uint8_t USART_Mode;						/* Specifies the USART Modes whether Tx, Rx or Tx and Rx Together */
	uint32_t USART_Baud;					/* Specifies the USART Baud rates desired */
	uint8_t USART_StopBits;					/* Specifies the USART Number of Stop bits */
	uint8_t USART_WordLength;				/* Specifies the USART Word length whether 8 bit or 9 bit */
	uint8_t USART_ParityControl;			/* Specifies the USART Parity Control */
	uint8_t USART_HWFlowControl;			/* Specifies the USART Hardware Flow Control */

}USART_Config_t;


/*
 * Handle structure for USARTx peripheral
 */
typedef struct
{
	USART_RegDef_t  *pUSARTx;			/*This holds the base address of USARTx Peripheral	*/
	USART_Config_t   USARTConfig;		/*This holds USARTx Peripheral configuration settings*/

	uint8_t *pTxBuffer;					/*  To store the application Tx buffer address > */
	uint8_t *pRxBuffer;					/*  To store the application Rx buffer address > */
	uint32_t TxLen;						/*  To store Tx len > */
	uint32_t RxLen;						/*  To store Rx len > */
	uint8_t TxBusyState;				/*  To store Tx state > */
	uint8_t RxBusyState;				/*  To store Rx state > */

}USART_Handle_t;



/*
 * @USART_Mode
 */
#define USART_MODE_TX 		0
#define USART_MODE_RX 		1
#define USART_MODE_TXRX  	2


/*
 * @USART_Baud
 */
#define USART_STD_BAUD_1200					1200
#define USART_STD_BAUD_2400					2400
#define USART_STD_BAUD_9600					9600  //Commonly used
#define USART_STD_BAUD_19200 				19200
#define USART_STD_BAUD_38400 				38400
#define USART_STD_BAUD_57600 				57600
#define USART_STD_BAUD_115200 				115200
#define USART_STD_BAUD_230400 				230400
#define USART_STD_BAUD_460800 				460800
#define USART_STD_BAUD_921600 				921600
#define USART_STD_BAUD_2M 					2000000
#define SUART_STD_BAUD_3M 					3000000


/*
 *@USART_ParityControl
 */
#define USART_PARITY_EN_ODD   2
#define USART_PARITY_EN_EVEN  1
#define USART_PARITY_DISABLE  0 //Default

/*
 *@USART_WordLength
 */
#define USART_WORDLEN_8BITS  0 //Default
#define USART_WORDLEN_9BITS  1

/*
 *@USART_StopBits
 */
#define USART_STOPBITS_1     0 //By default
#define USART_STOPBITS_0_5   1
#define USART_STOPBITS_2     2
#define USART_STOPBITS_1_5   3


/*
 *@USART_HWFlowControl
 */
#define USART_HW_FLOW_CTRL_NONE    	0
#define USART_HW_FLOW_CTRL_CTS    	1
#define USART_HW_FLOW_CTRL_RTS    	2
#define USART_HW_FLOW_CTRL_CTS_RTS	3


/*
 * USART Status Flag Definitions
 */
#define USART_FLAG_TXE 			( 1 << USART_SR_TXE)
#define USART_FLAG_RXNE 		( 1 << USART_SR_RXNE)
#define USART_FLAG_TC 			( 1 << USART_SR_TC)//Tranmission Complete
#define USART_FLAG_PE        	( 1 << USART_SR_PE)//Parity Error
#define USART_FLAG_FE        	( 1 << USART_SR_FE)//Framing Error
#define USART_FLAG_NF        	( 1 << USART_SR_NF)//Noise detected Flag

/*
 * USART Application states
 */
#define USART_BUSY_RX 	1
#define USART_BUSY_TX	2
#define USART_READY	  	0

/*
 * Possible USART Application events
 */
#define 	USART_EVENT_TX_CMPLT   0
#define		USART_EVENT_RX_CMPLT   1
#define		USART_EVENT_IDLE       2
#define		USART_EVENT_CTS        3
#define		USART_EVENT_PE         4 //Parity error
#define		USART_ERR_FE     	   5 //Framing error
#define		USART_ERR_NE    	   6 //Noise error
#define		USART_ERR_ORE    	   7 //Overrun error



/******************************************************************************/
/*                  Function Prototype by this driver - APIs	              */
/******************************************************************************/

/*
 * Peripheral Clock setup
 */
void USART_PCLK(USART_RegDef_t *pUSARTx, uint8_t EnOrDis);

/*
 * Init and De-init
 */
void USART_Init(USART_Handle_t *pUSARTHandle);
void USART_DeInit(USART_RegDef_t *pUSARTx);

/*
 * Data Send and Receive
 */
void USART_SendData(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len);
void  USART_ReceiveData(USART_Handle_t *pUSARTHandle,uint8_t *pRxBuffer, uint32_t Len);

uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pTxBuffer, uint32_t Len);
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pRxBuffer, uint32_t Len);

/*
 * IRQ Configuration and ISR handling
 */
void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDis);
void USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void USART_IRQHandling(USART_Handle_t *pUSARTHandle);


/*
 * Other Peripheral Control APIs
 */
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint8_t FlagName);
void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint16_t FlagName);
void USART_PeriphControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDis);
void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate);


/*
 * Application Callbacks
 */
void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t AppEv);


#endif /* INC_STM32F100XX_USART_H_ */
