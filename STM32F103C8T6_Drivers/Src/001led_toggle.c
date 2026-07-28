/*
 * 001led_toggle.c
 *
 *  Created on: Jul 6, 2026
 *      Author: Luji
 */

#include "../Drivers/Inc/stm32f103c8t6.h"


void delay(void)
{
	for(uint32_t i = 0 ; i < 500000 ; i ++);
}


int main(void)
{

	GPIO_Handle_t GpioLed;

	//PC13 là led xanh internal
	GpioLed.pGPIOx = GPIOB;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_OP_PP_50;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GpioLed);

	while(1)
	{
		GPIO_ToggleOutputPin(GPIOB,GPIO_PIN_NO_12);
		delay();
	}
	return 0;
}

