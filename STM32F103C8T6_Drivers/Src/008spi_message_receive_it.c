/*
 * 008spi_message_receive_it.c
 *
 *  Created on: Jul 23, 2026
 *      Author: Luji
 */

/*
 * This application receives and prints the user message received from the Arduino peripheral in SPI interrupt mode
 * User sends the message through Arduino IDE's serial monitor tool
 * Monitor the message received in the SWV itm data console
 */
/*
 * Note : Follow the instructions to test this code
 * 1. Download this code on to STM32 board , acts as Master
 * 2. Download Slave code (003SPISlaveUartReadOverSPI.ino) on to Arduino board (Slave)
 * 3. Reset both the boards
 * 4. Enable SWV ITM data console to see the message
 * 5. Open Arduino IDE serial monitor tool
 * 6. Type anything and send the message (Make sure that in the serial monitor tool line ending set to carriage return)
 */

#include <stdio.h>
#include <string.h>
#include "../Drivers/Inc/stm32f103c8t6.h"

/*
 PB12 as SPI2_NSS
 PB13 as SPI2_SCLK
 PB14 as SPI2_MISO
 PB15 as SPI2_MOSI
 */

SPI_Handle_t SPI2Handle;

#define MAX_LEN 500

char RcvBuff[MAX_LEN];

volatile char ReadByte;

volatile uint8_t rcvStop = 0;

/*This flag will be set in the interrupt handler of the Arduino interrupt GPIO */
volatile uint8_t dataAvailable = 0;

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}

void SPI2_GPIOInits(void)
{
	GPIO_Handle_t SPIPins;
	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_OP_AF_PP_50;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	//SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	//MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);

	//MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPIPins);


	//NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPIPins);
}

void SPI2_Inits(void)
{
	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV32;
	SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8;
	SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_DI;						//Hardware slave management enabled for NSS pin

	SPI_Init(&SPI2Handle);
}

/*This function configures the GPIO pin over which SPI peripheral issues data available interrupt */
void Slave_GPIO_InterruptPinInit(void)
{
	GPIO_Handle_t spiIntPin;
	memset(&spiIntPin, 0, sizeof(spiIntPin));

	//this is led GPIO configuration
	spiIntPin.pGPIOx = GPIOD;
	spiIntPin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	spiIntPin.GPIO_PinConfig.GPIO_PinMode = GPIO_IN_IT_FT;
	spiIntPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PULL_UP;

	GPIO_Init(&spiIntPin);

	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, NVIC_IRQ_PRI15);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, ENABLE);
}

int main(void)
{
	uint8_t dummy = 0xff;
	Slave_GPIO_InterruptPinInit();
	SPI2_GPIOInits();								//This function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI2_Inits();									//This function is used to initialize the SPI2 peripheral parameters

	/*
	* making SSOE 1 does NSS output enable.
	* The NSS pin is automatically managed by the hardware.
	* i.e when SPE=1 , NSS will be pulled to low
	* and NSS pin will be high when SPE=0
	*/

	SPI_SSOEConfig(SPI2, ENABLE);
	SPI_IRQInterruptConfig(IRQ_NO_SPI2, ENABLE);

	while(1)
	{
		rcvStop = 0;
		while( ! dataAvailable);					//Wait until data available interrupt from transmitter device (slave)
		GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, DISABLE);
		SPI_PeripheralControl(SPI2, ENABLE);		//Enable the SPI2 peripheral

		while( ! rcvStop)
		{
			//Fetch the data from the SPI peripheral byte by byte in interrupt mode
			while(SPI_SendDataIT(&SPI2Handle, &dummy, 1) == SPI_BUSY_IN_TX);
			while(SPI_ReceiveDataIT(&SPI2Handle, &ReadByte, 1) == SPI_BUSY_IN_RX);
		}

		//Confirm SPI is not busy
		while(SPI_GetFlagStatus(SPI2, SPI_BSY_FLAG));

		//Disable the SPI2 peripheral
		SPI_PeripheralControl(SPI2, DISABLE);
		printf("Received Data = %s\n", RcvBuff);
		dataAvailable = 0;
		GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, ENABLE);
	}
	return 0;
}

/* Runs when a data byte is received from the peripheral over SPI*/
void SPI2_IRQHandler(void)
{
	SPI_IRQHandling(&SPI2Handle);
}

void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
	static uint32_t i = 0;
	/* In the RX complete event , copy data in to RCV buffer . '\0' indicates end of message(rcvStop = 1) */
	if(AppEv == SPI_EVENT_RX_CMPLT)
	{
		RcvBuff[i++] = ReadByte;
		if(ReadByte == '\0' || ( i == MAX_LEN))
		{
			rcvStop = 1;
			RcvBuff[i-1] = '\0';
			i = 0;
		}
	}
}

/* Slave data available interrupt handler */
void EXTI9_5_IRQHandler(void)
{
	GPIO_IRQHandling(GPIO_PIN_NO_6);
	dataAvailable = 1;
}




















