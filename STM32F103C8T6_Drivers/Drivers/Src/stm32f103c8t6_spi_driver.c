/*
 * stm32f103c8t6_spi_driver.c
 *
 *  Created on: Jul 17, 2026
 *      Author: Luji
 */

#include "../Drivers/Inc/stm32f103c8t6_spi_driver.h"

static void  spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void  spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void  spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);

/*********************************************************************
 * @fn      		  - SPI_PeriClockControl
 *
 * @brief             - This function enables or disables peripheral clock for the SPI
 *
 * @param[in]         - base address of the SPI peripheral
 * @param[in]         - ENABLE or DISABLE macros
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_EN();
		}else if (pSPIx == SPI2)
		{
			SPI2_PCLK_EN();
		}else if (pSPIx == SPI3)
		{
			SPI3_PCLK_EN();
		}
	}
	else
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_DI();
		}else if (pSPIx == SPI2)
		{
			SPI2_PCLK_DI();
		}else if (pSPIx == SPI3)
		{
			SPI3_PCLK_DI();
		}
	}
}



/*********************************************************************
 * @fn      		  - SPI_Init
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -
 */
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	//Enable the Peripheral Clock
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	uint32_t tempReg = 0;

	//1. Configure the device mode
	tempReg |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	//2. Configure the bus
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		//BIDI mode should be cleared
		tempReg &= ~(1 << SPI_CR1_BIDIMODE);
	}else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		//BIDI mode should be set
		tempReg |= (1 << SPI_CR1_BIDIMODE);
	}else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_S_RX)
	{
		//BIDI mode should be cleared & RXONLY bit must be set
		tempReg &= ~(1 << SPI_CR1_BIDIMODE);
		tempReg |=  (1 << SPI_CR1_RXONLY);
	}

	//3. Configure the SPI serial clock speed (baud rate)
	tempReg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

	//4. Configure the DFF
	tempReg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;

	//5. Configure the CPOL
	tempReg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

	//6. Configure the CPHA
	tempReg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

	//7. Configure the SSM
	tempReg |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM;

	pSPIHandle->pSPIx->CR1 = tempReg;
}



/*********************************************************************
 * @fn      		  - SPI_DeInit
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -
 */
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	if(pSPIx == SPI1)
	{
		SPI1_REG_RESET();
	}else if(pSPIx == SPI2)
	{
		SPI2_REG_RESET();
	}else if(pSPIx == SPI3)
	{
		SPI3_REG_RESET();
	}
}

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if(pSPIx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}



/*********************************************************************
 * @fn      		  - SPI_SendData
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              - This is a blocking call
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	while(Len > 0)
	{
		//1. Wait until TXE is set
		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

		//2. Check the DFF bit in CR1
		if( (pSPIx->CR1 & (1 << SPI_CR1_DFF)) )
		{
			//16bits
			//1. Load the data into the DR
			pSPIx->DR = *((uint16_t*)pTxBuffer);
			Len--;
			Len--;
			(uint16_t*)pTxBuffer++;
		}else
		{
			//8bits
			//1. Load the data into the DR
			pSPIx->DR = *pTxBuffer;
			Len--;
			pTxBuffer++;
		}
	}
}



/*********************************************************************
 * @fn      		  - SPI_ReceiveData
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -
 */
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
	while(Len > 0)
	{
		//1. Wait until RXNE is set
		while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == (uint8_t)FLAG_RESET);

		//2. Check the DFF bit in CR1
		if( (pSPIx->CR1 & (1 << SPI_CR1_DFF)) )
		{
			//16bits DFF
			//Load the data from DR to RxBuffer address
			*((uint16_t*)pRxBuffer) = pSPIx->DR;
			Len--;
			Len--;
			(uint16_t*)pRxBuffer++;
		}else
		{
			//8bits
			//Load the data from DR to RxBuffer address
			*(pRxBuffer) = pSPIx->DR;
			Len--;
			pRxBuffer++;
		}
	}
}


/*********************************************************************
 * @fn      		  - SPI_PeripheralControl
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}



/*********************************************************************
 * @fn      		  - SPI_SSIConfig
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -
 */
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}



/*********************************************************************
 * @fn      		  - SPI_SSOEConfig
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -
 */
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	}else
	{
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}
}



//API to send data with interrupt mode
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;

	if(state != SPI_BUSY_IN_TX)
	{
		//1. Save the TX buffer address and Length information in some global variables
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;

		//2. Mark the SPI state as busy in transmission so that no other code can take over same SPI peripheral until transmission is over
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		//3. Enable the TXEIE control bit to get interrupt whenever the TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
	}

	return state;
}



uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->RxState;

	if(state != SPI_BUSY_IN_RX)
	{
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;
		pSPIHandle->RxState = SPI_BUSY_IN_RX;
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
	}

	return state;
}



/*********************************************************************
 * @fn      		  - SPI_IRQInterruptConfig
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			*NVIC_ISER0 |= (1 << IRQNumber);						//program ISER0 register
		}else if(IRQNumber > 31 && IRQNumber < 64) 					//32 to 63
		{
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));					//program ISER1 register
		}else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			*NVIC_ISER3 |= ( 1 << (IRQNumber % 64));				//program ISER2 register //64 to 95
		}
	}else
	{
		if(IRQNumber <= 31)
		{
			*NVIC_ICER0 |= (1 << IRQNumber);						//program ICER0 register
		}else if(IRQNumber > 31 && IRQNumber < 64)
		{
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));					//program ICER1 register
		}else if(IRQNumber >= 6 && IRQNumber < 96)
		{
			*NVIC_ICER3 |= (1 << (IRQNumber % 64));					//program ICER2 register
		}
	}
}



/*********************************************************************
 * @fn      		  - SPI_IRQPriorityConfig
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. first lets find out the IPR register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section  = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);

}



void SPI_IRQHandling(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp1, temp2;
	//1. Check the TXE flag
	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);
	if(temp1 && temp2)
	{
		//Handle TXE
		spi_txe_interrupt_handle(pSPIHandle);
	}

	//2. Check the RXNE flag
	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);
	if(temp1 && temp2)
	{
		//Handle RXNE
		spi_rxne_interrupt_handle(pSPIHandle);
	}

	//3. Check for OVR flag
	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_OVR);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if(temp1 && temp2)
	{
		//Handle OVR error
		spi_ovr_err_interrupt_handle(pSPIHandle);
	}
}



//Some helper functions implements
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	//Check the DFF bit in CR1
	if( (pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF)) )
	{
		//16bits DFF
		//1. load the data in to the DR
		pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen--;
		pSPIHandle->TxLen--;
		(uint16_t*)pSPIHandle->pTxBuffer++;
	}else
	{
		//8bits DFF
		pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer++;
	}

	if( ! pSPIHandle->TxLen)
	{
		//TxLen is zero, so close the SPI transmission & inform the application that TX is over
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
	}
}



static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	//Do RXING as per the DFF
	if(pSPIHandle->pSPIx->CR1 & (1 << 11))
	{
		//16bits
		*((uint16_t*)pSPIHandle->pRxBuffer) = (uint16_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen -= 2;
		pSPIHandle->pRxBuffer++;
		pSPIHandle->pRxBuffer++;
	}else
	{
		//8bits
		*(pSPIHandle->pRxBuffer) = (uint8_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->pRxBuffer++;
	}

	if( ! pSPIHandle->RxLen)
	{
		//Reception is complete, close the reception
		SPI_CloseReception(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
	}
}



static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp;

	//1. Clear the OVR flag
	if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
	{
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
	}
	(void)temp;
	//2. Inform the application
	SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);
}



void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);			//This is prevents interrupts from setting up of TXE flag
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;
}



void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);			//Disable the RXNEIE
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
}



void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void)temp;
}



/*__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t AppEv)
{
	//This is a weak implementation . the user application may override this function.

}
*/



















