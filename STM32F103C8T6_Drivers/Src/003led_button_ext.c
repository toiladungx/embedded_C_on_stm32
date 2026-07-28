/*
 * 003led_button_ext.c
 *
 *  Created on: Jul 10, 2026
 *      Author: Luji
 */

#include "../Drivers/Inc/stm32f103c8t6.h"

#define HIGH 1
#define LOW 0
#define BTN_PRESSED LOW

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000 ; i ++);
}

int main(void)
{
	GPIO_Handle_t GpioLed, GpioBtn;

	//LED config
	GpioLed.pGPIOx = GPIOB;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_OP_PP_50;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GpioLed);

	//Button config
	GpioBtn.pGPIOx = GPIOA;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_IN_PUPD;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PULL_UP;
	GPIO_Init(&GpioBtn);

	while(1)
	{
		if(GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NO_8) == BTN_PRESSED)
		{
				delay();
				GPIO_ToggleOutputPin(GPIOB,GPIO_PIN_NO_12);
		}
	}
	return 0;
}
