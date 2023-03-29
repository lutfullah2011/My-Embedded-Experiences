################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP(Board\ Support\ Package)/ds1307.c \
../BSP(Board\ Support\ Package)/lcd.c 

OBJS += \
./BSP(Board\ Support\ Package)/ds1307.o \
./BSP(Board\ Support\ Package)/lcd.o 

C_DEPS += \
./BSP(Board\ Support\ Package)/ds1307.d \
./BSP(Board\ Support\ Package)/lcd.d 


# Each subdirectory must supply rules for building sources it contributes
BSP(Board\ Support\ Package)/ds1307.o: ../BSP(Board\ Support\ Package)/ds1307.c BSP(Board\ Support\ Package)/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DDEBUG -DSTM32VLDISCOVERY -DSTM32 -DSTM32F1 -DSTM32F100RBTx -c -I../Inc -I"C:/Users/luty1/OneDrive/Belgeler/Embedded-C/My_workspace/target/001Led_Toggle_API/Drivers/Inc" -I"C:/Users/luty1/OneDrive/Belgeler/Embedded-C/My_workspace/target/001Led_Toggle_API" -I"C:/Users/luty1/OneDrive/Belgeler/Embedded-C/My_workspace/target/001Led_Toggle_API/Startup" -I"C:/Users/luty1/OneDrive/Belgeler/Embedded-C/My_workspace/target/001Led_Toggle_API/BSP(Board Support Package)" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"BSP(Board Support Package)/ds1307.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
BSP(Board\ Support\ Package)/lcd.o: ../BSP(Board\ Support\ Package)/lcd.c BSP(Board\ Support\ Package)/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DDEBUG -DSTM32VLDISCOVERY -DSTM32 -DSTM32F1 -DSTM32F100RBTx -c -I../Inc -I"C:/Users/luty1/OneDrive/Belgeler/Embedded-C/My_workspace/target/001Led_Toggle_API/Drivers/Inc" -I"C:/Users/luty1/OneDrive/Belgeler/Embedded-C/My_workspace/target/001Led_Toggle_API" -I"C:/Users/luty1/OneDrive/Belgeler/Embedded-C/My_workspace/target/001Led_Toggle_API/Startup" -I"C:/Users/luty1/OneDrive/Belgeler/Embedded-C/My_workspace/target/001Led_Toggle_API/BSP(Board Support Package)" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"BSP(Board Support Package)/lcd.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-BSP-28-Board-20-Support-20-Package-29-

clean-BSP-28-Board-20-Support-20-Package-29-:
	-$(RM) ./BSP(Board\ Support\ Package)/ds1307.cyclo ./BSP(Board\ Support\ Package)/ds1307.d ./BSP(Board\ Support\ Package)/ds1307.o ./BSP(Board\ Support\ Package)/ds1307.su ./BSP(Board\ Support\ Package)/lcd.cyclo ./BSP(Board\ Support\ Package)/lcd.d ./BSP(Board\ Support\ Package)/lcd.o ./BSP(Board\ Support\ Package)/lcd.su

.PHONY: clean-BSP-28-Board-20-Support-20-Package-29-

