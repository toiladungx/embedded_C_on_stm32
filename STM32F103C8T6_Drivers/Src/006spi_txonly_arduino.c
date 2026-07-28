/*
 * 006spi_txonly_arduino.c
 *
 *  Created on: Jul 20, 2026
 *      Author: Luji
 */

/*
 *
 */

#include <string.h>
#include "../Drivers/Inc/stm32f103c8t6.h"

/*
 PB12 as SPI2_NSS
 PB13 as SPI2_SCLK
 PB14 as SPI2_MISO
 PB15 as SPI2_MOSI
 */

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000 ; i ++);
}

void SPI2_GPIOInit(void)
{
	GPIO_Handle_t SPI_Pin;

	SPI_Pin.pGPIOx = GPIOB;
	SPI_Pin.GPIO_PinConfig.GPIO_PinMode = GPIO_OP_AF_PP_50; 			//Alternate Function Push-Pull (TX UART, SPI, PWM...)
	SPI_Pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	//SCLK
	SPI_Pin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPI_Pin);

	//MOSI
	SPI_Pin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPI_Pin);

	//NSS
	SPI_Pin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPI_Pin);
}

void SPI2_Inits(void)
{
	SPI_Handle_t SPI2Handle;

	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV8; //Generates SCLK of 2MHz div8
	SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8;
	SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_DI;

	SPI_Init(&SPI2Handle);
}

void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GpioBtn;

	//Button configure
	GpioBtn.pGPIOx = GPIOA;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_IN_PUPD;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PULL_DOWN;
	GPIO_Init(&GpioBtn);
}

int main(void)
{
	char user_data[] = "Hello World!";

	GPIO_ButtonInit();

	//This function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI2_GPIOInit();

	//This function is used to initialize the SPI2 peripheral parameters
	SPI2_Inits();

	SPI_SSOEConfig(SPI2, ENABLE);

	while(1)
	{
		//Wait until button is pressed
		while(! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		//Avoids button de-bouncing
		delay();

		//Enable the SPI2 peripheral
		SPI_PeripheralControl(SPI2, ENABLE);

		//Send length info
		uint8_t dataLen = strlen(user_data);
		SPI_SendData(SPI2, &dataLen, 1);

		//Send data
		SPI_SendData(SPI2, (uint8_t*)user_data, dataLen);

		//Confirm SPI is not busy
		while(SPI_GetFlagStatus(SPI2, SPI_BSY_FLAG));

		SPI_ClearOVRFlag(SPI2);

		//Disable the SPI2 peripheral
		SPI_PeripheralControl(SPI2, DISABLE);
	}
	return 0;
}







