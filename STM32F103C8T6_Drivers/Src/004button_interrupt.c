/*
 * 004button_interrupt.c
 *
 *  Created on: Jul 11, 2026
 *      Author: Luji
 */

#include "../Drivers/Inc/stm32f103c8t6.h"


#include <string.h>

#define HIGH 1
#define LOW 0
#define BTN_PRESSED LOW

void delay(void)
{
	// this will introduce ~200ms delay when system clock is 16MHz
	for(uint32_t i = 0 ; i < 200000 ; i ++);
}

int main(void)
{

	GPIO_Handle_t GpioLed, GpioBtn;

	memset(&GpioLed,0,sizeof(GpioLed));
	memset(&GpioBtn,0,sizeof(GpioBtn));

	//this is led gpio configuration
	GpioLed.pGPIOx = GPIOB;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_OP_PP_50;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GpioLed);

	//this is btn gpio configuration
	GpioBtn.pGPIOx = GPIOA;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_IN_IT_FT;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PULL_UP;
	GPIO_Init(&GpioBtn);

	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, GPIO_PIN_RESET);
	//IRQ configurations
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, NVIC_IRQ_PRI15);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, ENABLE);

    while(1){}

}


void EXTI9_5_IRQHandler(void)
{
	delay(); //200ms . wait till button de-bouncing gets over
	GPIO_IRQHandling(GPIO_PIN_NO_8); //clear the pending event from exti line
	GPIO_ToggleOutputPin(GPIOB, GPIO_PIN_NO_12);
}
