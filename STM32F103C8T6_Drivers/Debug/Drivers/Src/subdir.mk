################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Src/stm32f103c8t6_gpio_driver.c \
../Drivers/Src/stm32f103c8t6_i2c_driver.c \
../Drivers/Src/stm32f103c8t6_spi_driver.c 

OBJS += \
./Drivers/Src/stm32f103c8t6_gpio_driver.o \
./Drivers/Src/stm32f103c8t6_i2c_driver.o \
./Drivers/Src/stm32f103c8t6_spi_driver.o 

C_DEPS += \
./Drivers/Src/stm32f103c8t6_gpio_driver.d \
./Drivers/Src/stm32f103c8t6_i2c_driver.d \
./Drivers/Src/stm32f103c8t6_spi_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Src/%.o Drivers/Src/%.su Drivers/Src/%.cyclo: ../Drivers/Src/%.c Drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I../Inc -I"D:/LEARNING/STM32/Projects/STM32F103C8T6/STM32F103C8T6_Drivers/Drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-Src

clean-Drivers-2f-Src:
	-$(RM) ./Drivers/Src/stm32f103c8t6_gpio_driver.cyclo ./Drivers/Src/stm32f103c8t6_gpio_driver.d ./Drivers/Src/stm32f103c8t6_gpio_driver.o ./Drivers/Src/stm32f103c8t6_gpio_driver.su ./Drivers/Src/stm32f103c8t6_i2c_driver.cyclo ./Drivers/Src/stm32f103c8t6_i2c_driver.d ./Drivers/Src/stm32f103c8t6_i2c_driver.o ./Drivers/Src/stm32f103c8t6_i2c_driver.su ./Drivers/Src/stm32f103c8t6_spi_driver.cyclo ./Drivers/Src/stm32f103c8t6_spi_driver.d ./Drivers/Src/stm32f103c8t6_spi_driver.o ./Drivers/Src/stm32f103c8t6_spi_driver.su

.PHONY: clean-Drivers-2f-Src

