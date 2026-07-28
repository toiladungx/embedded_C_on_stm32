/*
 * stm32f103c8t6_gpio_driver.c
 *
 *  Created on: Jul 6, 2026
 *      Author: Luji
 */

#include "../Drivers/Inc/stm32f103c8t6_gpio_driver.h"

/*********************************************************************
 * @fn      		  - GPIO_PeriClockControl
 *
 * @brief             - This function enables or disables peripheral clock for the given GPIO port
 *
 * @param[in]         - base address of the GPIO peripheral
 * @param[in]         - ENABLE or DISABLE macros
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_EN();
		}else if (pGPIOx == GPIOB)
		{
			GPIOB_PCLK_EN();
		}else if (pGPIOx == GPIOC)
		{
			GPIOC_PCLK_EN();
		}else if (pGPIOx == GPIOD)
		{
			GPIOD_PCLK_EN();
		}else if (pGPIOx == GPIOE)
		{
			GPIOE_PCLK_EN();
		}
	}
	else
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_DI();
		}else if (pGPIOx == GPIOB)
		{
			GPIOB_PCLK_DI();
		}else if (pGPIOx == GPIOC)
		{
			GPIOC_PCLK_DI();
		}else if (pGPIOx == GPIOD)
		{
			GPIOD_PCLK_DI();
		}else if (pGPIOx == GPIOE)
		{
			GPIOE_PCLK_DI();
		}
	}
}



/*********************************************************************
 * @fn      		  - GPIO_Init
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


void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
    uint32_t bitShift = 0;

    // 1. Enable Peripheral Clock
    GPIO_PeriClockControl(pGPIOHandle->pGPIOx, ENABLE);

    // 2. Configure the mode of GPIO pin (Input / Output / Alternate Function)
    if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_OP_AF_OD_50)
    {
        //Check CRL register or CRH register
        if(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber < 8)
        {	//CRL
            bitShift = (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) * 4;
            pGPIOHandle->pGPIOx->CRL &= ~(0xF << bitShift); //Clearing 4 bits of pin
            pGPIOHandle->pGPIOx->CRL |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << bitShift);
        }
        else
        {	//CRH
            bitShift = (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8) * 4;
            pGPIOHandle->pGPIOx->CRH &= ~(0xF << bitShift); //Clearing 4 bits of pin
            pGPIOHandle->pGPIOx->CRH |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << bitShift);
        }

        //If PUPD -> Configure by ODR
        if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_IN_PUPD)
        {
            if(pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PULL_UP)
            {
                pGPIOHandle->pGPIOx->ODR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); // Pull-Up
            }
            else
            {
                pGPIOHandle->pGPIOx->ODR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); // Pull-Down or No-PUPD
            }
        }
    }
    else
    {
        // 3. Configure the Interrupt Mode
        uint8_t inputConfig = GPIO_IN_PUPD;

        if(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber < 8)
        {	//CRL
            bitShift = (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) * 4;
            pGPIOHandle->pGPIOx->CRL &= ~(0xF << bitShift);
            pGPIOHandle->pGPIOx->CRL |= (inputConfig << bitShift);
        }
        else
        {	//CRH
            bitShift = (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8) * 4;
            pGPIOHandle->pGPIOx->CRH &= ~(0xF << bitShift);
            pGPIOHandle->pGPIOx->CRH |= (inputConfig << bitShift);
        }

        //Configure PUPD by ODR
        if(pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PULL_UP)
        {
            pGPIOHandle->pGPIOx->ODR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //Pull-Up
        }
        else
        {
            pGPIOHandle->pGPIOx->ODR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //Pull-Down or No-PUPD
        }

        //Configure the EXTI FTSR / RTSR
        if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_IN_IT_FT)
        {
            EXTI->FTSR |=  (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //ON
            EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //OFF
        }
        else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_IN_IT_RT)
        {
            EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //OFF
            EXTI->RTSR |=  (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //ON
        }
        else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_IN_IT_RFT)
        {
            EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //ON
            EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //ON
        }

        //Configure the AFIO EXTICR
        uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4;
        uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;
        uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);

        AFIO_PCLK_EN();
        AFIO->EXTICR[temp1] &= ~(0xF << (temp2 * 4));
        AFIO->EXTICR[temp1] |= (portcode << (temp2 * 4));

        //Un-mask Interrupt
        EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    }
}



/*********************************************************************
 * @fn      		  - GPIO_ToggleOutputPin
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
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR  ^= ( 1 << PinNumber);
}



/*********************************************************************
 * @fn      		  - GPIO_DeInit
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
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if(pGPIOx == GPIOA)
	{
		GPIOA_REG_RESET();
	}else if (pGPIOx == GPIOB)
	{
		GPIOB_REG_RESET();
	}else if (pGPIOx == GPIOC)
	{
		GPIOC_REG_RESET();
	}else if (pGPIOx == GPIOD)
	{
		GPIOD_REG_RESET();
	}else if (pGPIOx == GPIOE)
	{
		GPIOE_REG_RESET();
	}

}



/*********************************************************************
 * @fn      		  - GPIO_ReadFromInputPin
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -   0 or 1
 *
 * @Note              -

 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
   uint8_t value;
   value = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001);
   return value;
}



/*********************************************************************
 * @fn      		  - GPIO_ReadFromInputPort
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
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
   uint16_t value;
   value = (uint16_t)pGPIOx->IDR;
   return value;
}



/*********************************************************************
 * @fn      		  - GPIO_WriteToOutputPin
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
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
		{
			//write 1 to the output data register at the bit field corresponding to the pin number
			pGPIOx->ODR |= ( 1 << PinNumber);
		}else
		{
			//write 0
			pGPIOx->ODR &= ~( 1 << PinNumber);
		}
}



/*********************************************************************
 * @fn      		  - GPIO_WriteToOutputPort
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
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value)
{
	pGPIOx->ODR  = Value;
}



/*********************************************************************
 * @fn      		  - GPIO_IRQConfig
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
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		//Interrupt SET-enable Registers
		if(IRQNumber <= 31)
		{
			//Program ISER0 register
			*NVIC_ISER0 |= (1 << IRQNumber);
		}
		else if(IRQNumber > 31 && IRQNumber < 64)
		{
			//Program ISER1 register
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));
		}
		else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			//Program ISER2 register
			*NVIC_ISER2 |= (1 << (IRQNumber % 64));
		}
	}else
	{
		//Interrupt CLEAR-enable Registers
		if(IRQNumber <= 31)
		{
			//Program ICER0 register
			*NVIC_ICER0 |= (1 << IRQNumber);
		}
		else if(IRQNumber > 31 && IRQNumber < 64)
		{
			//Program ICER1 register
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));
		}
		else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			//Program ICER2 register
			*NVIC_ICER2 |= (1 << (IRQNumber % 64));
		}
	}
}



/*********************************************************************
 * @fn      		  - IRQPriorityConfig
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
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	uint8_t IPRx = IRQNumber / 4;
	uint8_t IPRx_Section = IRQNumber % 4;
	uint8_t shift_amount = (8 * IPRx_Section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASE_ADDR + IPRx) |= (IRQPriority << shift_amount);
}




/*********************************************************************
 * @fn      		  - GPIO_IRQHandling
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
void GPIO_IRQHandling(uint8_t PinNumber)
{
	//clear the EXTI Pending Register corresponding to the pin number
	if(EXTI->PR & (1 << PinNumber))
	{
		//clear
		EXTI->PR |= (1 << PinNumber);
	}
}


