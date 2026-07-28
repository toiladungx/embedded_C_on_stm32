/*
 * stm32f103c8t6_spi_driver.h
 *
 *  Created on: Jul 17, 2026
 *      Author: Luji
 */

#ifndef INC_STM32F103C8T6_SPI_DRIVER_H_
#define INC_STM32F103C8T6_SPI_DRIVER_H_

#include "../Drivers/Inc/stm32f103c8t6.h"

//Configuration structure for SPIx peripheral
typedef struct
{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;
}SPI_Config_t;

//Handle structure for SPIx peripheral
typedef struct
{
	SPI_RegDef_t 	*pSPIx;   					//This holds the base address of SPIx(x:0,1,2) peripheral
	SPI_Config_t 	SPIConfig;
	uint8_t 		*pTxBuffer; 				//To store the application TX buffer address
	uint8_t 		*pRxBuffer;					//To store the application RX buffer address
	uint32_t 		TxLen;						//To store TX length
	uint32_t 		RxLen;						//To store RX length
	uint8_t 		TxState;					//To store TX state
	uint8_t 		RxState;					//To store RX state
}SPI_Handle_t;

//SPI application states
#define SPI_READY 								0
#define SPI_BUSY_IN_RX 							1
#define SPI_BUSY_IN_TX 							2

//Possible SPI application events
#define SPI_EVENT_TX_CMPLT   					1
#define SPI_EVENT_RX_CMPLT   					2
#define SPI_EVENT_OVR_ERR    					3
#define SPI_EVENT_CRC_ERR    					4

//@SPI_Device_Modes
#define SPI_DEVICE_MODE_SLAVE					0			//MSTR bit
#define SPI_DEVICE_MODE_MASTER					1

//@SPI_BusConfig
#define SPI_BUS_CONFIG_FD						1			//Full-duplex		bit15 = 0
#define SPI_BUS_CONFIG_HD						2			//Half-duplex		bit15 = 1
#define SPI_BUS_CONFIG_S_RX						3			//Simplex RX only	bit14 = 1

//@SPI_SclkSpeed
#define SPI_SCLK_SPEED_DIV2             		0			//BR bits
#define SPI_SCLK_SPEED_DIV4             		1
#define SPI_SCLK_SPEED_DIV8             		2
#define SPI_SCLK_SPEED_DIV16             		3
#define SPI_SCLK_SPEED_DIV32             		4
#define SPI_SCLK_SPEED_DIV64             		5
#define SPI_SCLK_SPEED_DIV128             		6
#define SPI_SCLK_SPEED_DIV256             		7

//@SPI_DFF
#define SPI_DFF_8 								0			//Data frame format: 8 or 16 bits
#define SPI_DFF_16  							1

//@CPOL
#define SPI_CPOL_LOW 							0
#define SPI_CPOL_HIGH 							1

//@CPHA
#define SPI_CPHA_LOW 							0
#define SPI_CPHA_HIGH 							1

//@SPI_SSM
#define SPI_SSM_DI     							0			//SSM: Software slave management
#define SPI_SSM_EN     							1



//SPI related status flags definitions
#define SPI_RXNE_FLAG							(1 << SPI_SR_RXNE)
#define SPI_TXE_FLAG							(1 << SPI_SR_TXE)
#define SPI_BSY_FLAG							(1 << SPI_SR_BSY)

/******************************************************************************************
 *								APIs supported by this driver
 *		 For more information about the APIs check the function definitions
 ******************************************************************************************/

 //Peripheral Clock setup
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

//Init and De-init
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);

//Data read and write
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len);
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len);

//IRQ Configuration and ISR handling
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle);

//Other Peripheral Control APIs
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi);
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi);
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName);
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);
uint8_t I2C_DeviceMode(I2C_RegDef_t *I2Cx);

//Application callback
void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv);

#endif /* INC_STM32F103C8T6_SPI_DRIVER_H_ */
