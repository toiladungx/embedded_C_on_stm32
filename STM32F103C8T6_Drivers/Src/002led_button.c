/*
 * 002led_button.c
 *
 *  Created on: Jul 10, 2026
 *      Author: Luji
 */

#include "../Drivers/Inc/stm32f103c8t6.h"

#define HIGH 1
#define BTN_PRESSED HIGH

void delay(void)
{
	for(uint32_t i = 0 ; i < 250000 ; i ++);
}

int main(void)
{

	GPIO_Handle_t GpioLed, GpioBtn;

	//LED config
	GpioLed.pGPIOx = GPIOC;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_OP_PP_50;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GpioLed);

	//Btn config
	GpioBtn.pGPIOx = GPIOA;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_IN_PUPD;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GpioBtn);

	while(1)
	{
		if(GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NO_8) == BTN_PRESSED)
		{
			delay();
			GPIO_ToggleOutputPin(GPIOC,GPIO_PIN_NO_13);
		}
	}
	return 0;
}

