################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/lr_fhss_mac.c \
../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio.c \
../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_driver.c \
../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_fw.c \
../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/wl_lr_fhss.c 

OBJS += \
./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/lr_fhss_mac.o \
./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio.o \
./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_driver.o \
./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_fw.o \
./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/wl_lr_fhss.o 

C_DEPS += \
./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/lr_fhss_mac.d \
./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio.d \
./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_driver.d \
./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_fw.d \
./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/wl_lr_fhss.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/%.o Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/%.su Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/%.cyclo: ../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/%.c Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../Core/Inc -I../SubGHz_Phy/App -I../SubGHz_Phy/Target -I../Utilities/trace/adv_trace -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Utilities/misc -I../Utilities/sequencer -I../Utilities/timer -I../Utilities/lpm/tiny_lpm -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Middlewares/Third_Party/SubGHz_Phy -I../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../Drivers/CMSIS/Include -I"/Users/vlkjan/Documents/projekty/CVUT/Ing/2. Semestr/SPD/nucleo-joy/LoRa_joy/Drivers/stm32wlxx-nucleo-bsp" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-SubGHz_Phy-2f-stm32_radio_driver

clean-Middlewares-2f-Third_Party-2f-SubGHz_Phy-2f-stm32_radio_driver:
	-$(RM) ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/lr_fhss_mac.cyclo ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/lr_fhss_mac.d ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/lr_fhss_mac.o ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/lr_fhss_mac.su ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio.cyclo ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio.d ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio.o ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio.su ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_driver.cyclo ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_driver.d ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_driver.o ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_driver.su ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_fw.cyclo ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_fw.d ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_fw.o ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/radio_fw.su ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/wl_lr_fhss.cyclo ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/wl_lr_fhss.d ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/wl_lr_fhss.o ./Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver/wl_lr_fhss.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-SubGHz_Phy-2f-stm32_radio_driver

