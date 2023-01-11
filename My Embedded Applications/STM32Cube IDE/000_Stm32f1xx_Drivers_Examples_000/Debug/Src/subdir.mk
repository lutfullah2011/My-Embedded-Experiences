################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/007SPI_Rx_message_from_Ardiuno_InterruptMode.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/007SPI_Rx_message_from_Ardiuno_InterruptMode.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/007SPI_Rx_message_from_Ardiuno_InterruptMode.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DDEBUG -DSTM32VLDISCOVERY -DSTM32 -DSTM32F1 -DSTM32F100RBTx -c -I../Inc -I"C:/Users/luty1/OneDrive/Belgeler/Embedded-C/My_workspace/target/001Led_Toggle_API/Drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/007SPI_Rx_message_from_Ardiuno_InterruptMode.d ./Src/007SPI_Rx_message_from_Ardiuno_InterruptMode.o ./Src/007SPI_Rx_message_from_Ardiuno_InterruptMode.su ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

