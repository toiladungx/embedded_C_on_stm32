/*
 * stm32f103c8t6_gpio_driver.h
 *
 *  Created on: Jul , 2026
 *      Author: Luji
 */

#ifndef DRIVERS_INC_STM32F103C8T6_GPIO_DRIVER_H_
#define DRIVERS_INC_STM32F103C8T6_GPIO_DRIVER_H_


#include "../Drivers/Inc/stm32f103c8t6.h"


// This is a Configuration structure for a GPIO pin
typedef struct
{
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;			/*!< possible values from @GPIO_PIN_MODES >*/
	uint8_t GPIO_PinPuPdControl;
}GPIO_PinConfig_t;

// This is a Handle structure for a GPIO pin
typedef struct
{
	GPIO_RegDef_t *pGPIOx;       		/*!< This holds the base address of the GPIO port to which the pin belongs >*/
	GPIO_PinConfig_t GPIO_PinConfig;   /*!< This holds GPIO pin configuration settings >*/
}GPIO_Handle_t;

// GPIO pin numbers @GPIO_PIN_NUMBERS
#define GPIO_PIN_NO_0  				0
#define GPIO_PIN_NO_1  				1
#define GPIO_PIN_NO_2  				2
#define GPIO_PIN_NO_3  				3
#define GPIO_PIN_NO_4  				4
#define GPIO_PIN_NO_5  				5
#define GPIO_PIN_NO_6  				6
#define GPIO_PIN_NO_7  				7
#define GPIO_PIN_NO_8  				8
#define GPIO_PIN_NO_9  				9
#define GPIO_PIN_NO_10  			10
#define GPIO_PIN_NO_11 				11
#define GPIO_PIN_NO_12  			12
#define GPIO_PIN_NO_13 				13
#define GPIO_PIN_NO_14 				14
#define GPIO_PIN_NO_15 				15



/****************************GPIO PIN MODES DEFINITIONS****************************/
//								@GPIO_PIN_MODES
//						[ CNF1 ][ CNF0 ][ MODE1 ][ MODE0 ]

//GPIO pin possible input types (MODE == [00])
#define GPIO_IN_ANALOG      		0x00  		// 0000b : Analog input mode (ADC)
#define GPIO_IN_FLOATING    		0x04  		// 0100b : Floating input (reset state)
#define GPIO_IN_PUPD        		0x08  		// 1000b : Input with pull-up / pull-down

#define GPIO_IN_IT_FT            	0x10  		// Interrupt Falling Edge Trigger
#define GPIO_IN_IT_RT             	0x11  		// Interrupt Rising Edge Trigger
#define GPIO_IN_IT_RFT            	0x12  		// Interrupt Rising-Falling Edge Trigger

//GPIO pin possible output types low speed (10MHz - MODE == [01])
#define GPIO_OP_PP_10      			0x01  		// 0001b : Output Push-Pull
#define GPIO_OP_OD_10      			0x05  		// 0101b : Output Open-Drain
#define GPIO_OP_AF_PP_10   			0x09  		// 1001b : Alternate Function Push-Pull (TX UART, SPI, PWM...)
#define GPIO_OP_AF_OD_10   			0x0D  		// 1101b : Alternate Function Open-Drain (I2C...)

//GPIO pin possible output types medium speed (2MHz - MODE == [10])
#define GPIO_OP_PP_2       			0x02  		// 0010b
#define GPIO_OP_OD_2       			0x06  		// 0110b
#define GPIO_OP_AF_PP_2    			0x0A  		// 1010b
#define GPIO_OP_AF_OD_2    			0x0E  		// 1110b

//GPIO pin possible output types high speed (50MHz - MODE == [11])
#define GPIO_OP_PP_50      			0x03 		// 0011b
#define GPIO_OP_OD_50      			0x07  		// 0111b
#define GPIO_OP_AF_PP_50   			0x0B  		// 1011b
#define GPIO_OP_AF_OD_50   			0x0F  		// 1111b

//GPIO pin pull up AND pull down configuration macros
#define GPIO_NO_PUPD                0
#define GPIO_PULL_DOWN              0
#define GPIO_PULL_UP                1



/******************************************************************************************
 *								APIs supported by this driver
 *		 For more information about the APIs check the function definitions
 ******************************************************************************************/

 //Peripheral Clock setup
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi);

//Init and De-init
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

//Data read and write
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);

//IRQ Configuration and ISR handling
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);


#endif /* DRIVERS_INC_STM32F103C8T6_GPIO_DRIVER_H_ */
