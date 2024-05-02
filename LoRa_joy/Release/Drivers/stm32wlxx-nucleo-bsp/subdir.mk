################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo.c \
../Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo_radio.c 

OBJS += \
./Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo.o \
./Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo_radio.o 

C_DEPS += \
./Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo.d \
./Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo_radio.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/stm32wlxx-nucleo-bsp/%.o Drivers/stm32wlxx-nucleo-bsp/%.su Drivers/stm32wlxx-nucleo-bsp/%.cyclo: ../Drivers/stm32wlxx-nucleo-bsp/%.c Drivers/stm32wlxx-nucleo-bsp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../Core/Inc -I../SubGHz_Phy/App -I../SubGHz_Phy/Target -I../Utilities/trace/adv_trace -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Utilities/misc -I../Utilities/sequencer -I../Utilities/timer -I../Utilities/lpm/tiny_lpm -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Middlewares/Third_Party/SubGHz_Phy -I../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../Drivers/CMSIS/Include -I"/Users/vlkjan/Documents/projekty/CVUT/Ing/2.Semestr/SPD/nucleo-joy/LoRa_joy/Drivers/stm32wlxx-nucleo-bsp" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-stm32wlxx-2d-nucleo-2d-bsp

clean-Drivers-2f-stm32wlxx-2d-nucleo-2d-bsp:
	-$(RM) ./Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo.cyclo ./Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo.d ./Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo.o ./Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo.su ./Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo_radio.cyclo ./Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo_radio.d ./Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo_radio.o ./Drivers/stm32wlxx-nucleo-bsp/stm32wlxx_nucleo_radio.su

.PHONY: clean-Drivers-2f-stm32wlxx-2d-nucleo-2d-bsp

