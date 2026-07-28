/*
 * 005spi_tx_testing.c
 *
 *  Created on: Jul 17, 2026
 *      Author: Luji
 */

/*
 * Test the SPI_SendData API to send the string "Hello World" and use the bellow configurations
 * 1. SPI2 Master Mode
 * 2. SCLK = Max possible
 * 3. DFF = 0 and DFF = 1
 * Note: => No slave so only use 2 pins: SCLK and MOSI
 */

#include <string.h>
#include "../Drivers/Inc/stm32f103c8t6.h"

/*
 PB12 as SPI2_NSS
 PB13 as SPI2_SCLK
 PB14 as SPI2_MISO
 PB15 as SPI2_MOSI
 */

void SPI2_GPIOInits(void)
{
	GPIO_Handle_t SPI_Pins;

	SPI_Pins.pGPIOx = GPIOB;
	SPI_Pins.GPIO_PinConfig.GPIO_PinMode = GPIO_OP_AF_PP_50; //Alternate Function Push-Pull (TX UART, SPI, PWM...)
	SPI_Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	//SCLK
	SPI_Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPI_Pins);

	//MOSI
	SPI_Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPI_Pins);

}

void SPI2_Inits(void)
{
	SPI_Handle_t SPI2Handle;

	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2;
	SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8;
	SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_EN;

	SPI_Init(&SPI2Handle);
}

int main(void)
{
	char user_data[] = "Hello World";

	//This function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI2_GPIOInits();

	//This function is used to initialize the SPI2 peripheral parameters
	SPI2_Inits();

	//Make NSS signal internally high and avoids MODF error
	SPI_SSIConfig(SPI2, ENABLE);

	//Enable the SPI2 peripheral
	SPI_PeripheralControl(SPI2, ENABLE);

	SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));

	//Confirm SPI is not busy
	while(SPI_GetFlagStatus(SPI2, SPI_BSY_FLAG));

	//Disable the SPI2 peripheral
	SPI_PeripheralControl(SPI2, DISABLE);

	while(1);

	return 0;
}







