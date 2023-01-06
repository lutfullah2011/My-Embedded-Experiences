################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Src/stm32f100xx_gpio.c \
../Drivers/Src/stm32f100xx_spi.c 

OBJS += \
./Drivers/Src/stm32f100xx_gpio.o \
./Drivers/Src/stm32f100xx_spi.o 

C_DEPS += \
./Drivers/Src/stm32f100xx_gpio.d \
./Drivers/Src/stm32f100xx_spi.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Src/%.o Drivers/Src/%.su: ../Drivers/Src/%.c Drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DDEBUG -DSTM32VLDISCOVERY -DSTM32 -DSTM32F1 -DSTM32F100RBTx -c -I../Inc -I"C:/Users/luty1/OneDrive/Belgeler/Embedded-C/My_workspace/target/001Led_Toggle_API/Drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-Src

clean-Drivers-2f-Src:
	-$(RM) ./Drivers/Src/stm32f100xx_gpio.d ./Drivers/Src/stm32f100xx_gpio.o ./Drivers/Src/stm32f100xx_gpio.su ./Drivers/Src/stm32f100xx_spi.d ./Drivers/Src/stm32f100xx_spi.o ./Drivers/Src/stm32f100xx_spi.su

.PHONY: clean-Drivers-2f-Src

