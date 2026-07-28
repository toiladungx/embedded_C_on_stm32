/*
 * 007spi_cmd_handling.c
 *
 *  Created on: Jul 23, 2026
 *      Author: Luji
 */

#include <stdio.h>
#include <string.h>
#include "../Drivers/Inc/stm32f103c8t6.h"

extern void initialise_monitor_handles();

/*
 PB12 as SPI2_NSS
 PB13 as SPI2_SCLK
 PB14 as SPI2_MISO
 PB15 as SPI2_MOSI
 */

//command codes
#define COMMAND_LED_CTRL      		0x50
#define COMMAND_SENSOR_READ      	0x51
#define COMMAND_LED_READ      		0x52
#define COMMAND_PRINT      			0x53
#define COMMAND_ID_READ      		0x54

#define LED_ON     					1
#define LED_OFF    					0

//arduino analog pins
#define ANALOG_PIN0 				0
#define ANALOG_PIN1 				1
#define ANALOG_PIN2 				2
#define ANALOG_PIN3 				3
#define ANALOG_PIN4 				4

//arduino led
#define LED_PIN  					9

void delay(void)
{
	for(uint32_t i = 0 ; i < 250000 ; i ++);
}

void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GpioLed, GpioBtn;

	//Button configure
	GpioBtn.pGPIOx = GPIOA;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_IN_PUPD;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PULL_DOWN;
	GPIO_Init(&GpioBtn);

	//LED configure
	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_OP_OD_50;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_PeriClockControl(GPIOD,ENABLE);
	GPIO_Init(&GpioLed);
}

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

	//MISO
	SPI_Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPI_Pins);

	//NSS
	SPI_Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPI_Pins);
}

void SPI2_Inits(void)
{
	SPI_Handle_t SPI2Handle;

	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV32;
	SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8;
	SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_DI;

	SPI_Init(&SPI2Handle);
}

uint8_t SPI_VerifyResponse(uint8_t ackbyte)
{
	if (ackbyte == (uint8_t)0xF5)
	{
		//ACK
		return 1;
	}
	return 0;
}

int main(void)
{
	uint8_t dummy_write = 0xff;
	uint8_t dummy_read;

	GPIO_ButtonInit();

	//This function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI2_GPIOInits();

	//This function is used to initialize the SPI2 peripheral parameters
	SPI2_Inits();

	/*
	* making SSOE 1 does NSS output enable.
	* The NSS pin is automatically managed by the hardware.
	* i.e when SPE=1 , NSS will be pulled to low
	* and NSS pin will be high when SPE=0
	*/
	SPI_SSOEConfig(SPI2, ENABLE);

	while(1)
	{
		while(! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));			//Wait until button is pressed
		delay();														//Avoids button de-bouncing

		SPI_PeripheralControl(SPI2, ENABLE);							//Enable the SPI2 peripheral

		//1. CMD_LED_CTRL <pin no(1)> <value(1)>
		uint8_t cmdCode = COMMAND_LED_CTRL;
		uint8_t ackbyte;
		uint8_t args[2];

		SPI_SendData(SPI2, &cmdCode, 1);								//Send command
		SPI_ReceiveData(SPI2, &dummy_read, 1);							//Do dummy read to clear off the RXNE
		SPI_SendData(SPI2, &dummy_write, 1);							//Send some dummy bits (1 byte) fetch the response from the slave
		SPI_ReceiveData(SPI2, &ackbyte, 1);								//Read the ACK byte received

		if(SPI_VerifyResponse(ackbyte))
		{
			args[0] = LED_PIN;
			args[1] = LED_ON;
			SPI_SendData(SPI2, args, 2);								//Send arguments
			SPI_ReceiveData(SPI2, args, 2);								//Dummy read
			printf("COMMAND_LED_CTRL Executed\n");
		}



		//2. CMD_SENSOR_READ <analog pin number(1)>
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );
		delay();
		cmdCode = COMMAND_SENSOR_READ;
		SPI_SendData(SPI2, &cmdCode, 1);
		SPI_ReceiveData(SPI2, &dummy_read, 1);
		SPI_SendData(SPI2, &dummy_write, 1);
		SPI_ReceiveData(SPI2, &ackbyte, 1);

		if(SPI_VerifyResponse(ackbyte))
		{
			args[0] = ANALOG_PIN0;
			SPI_SendData(SPI2, args, 1);								//Send arguments
			SPI_ReceiveData(SPI2, &dummy_read, 1);						//Do dummy read to clear off the RXNE
			delay();													//Insert some delay so that slave can ready with the data
			SPI_SendData(SPI2, &dummy_write, 1);						//Send some dummy bits (1 byte) to fetch the response from the slave
			uint8_t analog_read;
			SPI_ReceiveData(SPI2, &analog_read, 1);
			printf("COMMAND_SENSOR_READ %d\n", analog_read);
		}



		//3. CMD_LED_READ <pin no(1)>
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );
		delay();
		cmdCode = COMMAND_LED_READ;
		SPI_SendData(SPI2, &cmdCode, 1);
		SPI_ReceiveData(SPI2, &dummy_read, 1);
		SPI_SendData(SPI2, &dummy_write, 1);
		SPI_ReceiveData(SPI2, &ackbyte, 1);

		if(SPI_VerifyResponse(ackbyte))
		{
			args[0] = LED_PIN;
			SPI_SendData(SPI2, args, 1);								//Send arguments
			SPI_ReceiveData(SPI2, &dummy_read, 1);						//Do dummy read to clear off the RXNE
			delay();													//Insert some delay so that slave can ready with the data
			SPI_SendData(SPI2, &dummy_write, 1);						//Send some dummy bits (1 byte) to fetch the response from the slave
			uint8_t led_status;
			SPI_ReceiveData(SPI2, &led_status, 1);
			printf("COMMAND_READ_LED %d\n", led_status);
		}



		//4. CMD_PRINT <len(2)> <message(len)>
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );
		delay();
		cmdCode = COMMAND_PRINT;
		SPI_SendData(SPI2, &cmdCode, 1);
		SPI_ReceiveData(SPI2, &dummy_read, 1);
		SPI_SendData(SPI2, &dummy_write, 1);
		SPI_ReceiveData(SPI2, &ackbyte, 1);
		uint8_t message[] = "Hello World";

		if(SPI_VerifyResponse(ackbyte))
		{
			args[0] = strlen((char*)message);
			SPI_SendData(SPI2, args, 1);								//Send arguments
			SPI_ReceiveData(SPI2, &dummy_read, 1);						//Do dummy read to clear off the RXNE
			delay();													//Insert some delay so that slave can ready with the data
			//Send message
			for(int i=0 ; i<args[0] ; i++)
			{
				SPI_SendData(SPI2, &message[i], 1);
				SPI_ReceiveData(SPI2, &dummy_read, 1);
			}
			printf("COMMAND_PRINT Executed \n");
		}



		//5. CMD_ID_READ
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );
		delay();
		cmdCode = COMMAND_ID_READ;
		SPI_SendData(SPI2, &cmdCode, 1);
		SPI_ReceiveData(SPI2, &dummy_read, 1);
		SPI_SendData(SPI2, &dummy_write, 1);
		SPI_ReceiveData(SPI2, &ackbyte, 1);
		uint8_t id[11];
		uint32_t i=0;

		if(SPI_VerifyResponse(ackbyte))
		{
			//read 10 bytes id from the slave
			for(i=0 ; i<10 ; i++)
			{
				//Send dummy byte to fetch data from slave
				SPI_SendData(SPI2, &dummy_write, 1);
				SPI_ReceiveData(SPI2, &id[i], 1);
			}

			id[10] = '\0';
			printf("COMMAND_ID : %s \n",id);
		}

		//Confirm SPI is not busy
		while(SPI_GetFlagStatus(SPI2, SPI_BSY_FLAG));
		for(volatile int i=0; i<500; i++);

		//Disable the SPI2 peripheral
		SPI_PeripheralControl(SPI2, DISABLE);

		printf("SPI Communication Closed\n");
	}
	return 0;
}

