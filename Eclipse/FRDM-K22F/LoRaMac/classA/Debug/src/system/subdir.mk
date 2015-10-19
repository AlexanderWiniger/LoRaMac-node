################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/workspace/eclipse/LoRaMac-node/src/system/adc.c \
C:/workspace/eclipse/LoRaMac-node/src/system/delay.c \
C:/workspace/eclipse/LoRaMac-node/src/system/eeprom.c \
C:/workspace/eclipse/LoRaMac-node/src/system/fifo.c \
C:/workspace/eclipse/LoRaMac-node/src/system/gpio.c \
C:/workspace/eclipse/LoRaMac-node/src/system/gps.c \
C:/workspace/eclipse/LoRaMac-node/src/system/i2c.c \
C:/workspace/eclipse/LoRaMac-node/src/system/timer.c \
C:/workspace/eclipse/LoRaMac-node/src/system/uart.c 

OBJS += \
./src/system/adc.o \
./src/system/delay.o \
./src/system/eeprom.o \
./src/system/fifo.o \
./src/system/gpio.o \
./src/system/gps.o \
./src/system/i2c.o \
./src/system/timer.o \
./src/system/uart.o 

C_DEPS += \
./src/system/adc.d \
./src/system/delay.d \
./src/system/eeprom.d \
./src/system/fifo.d \
./src/system/gpio.d \
./src/system/gps.d \
./src/system/i2c.d \
./src/system/timer.d \
./src/system/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/system/adc.o: C:/workspace/eclipse/LoRaMac-node/src/system/adc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/system/delay.o: C:/workspace/eclipse/LoRaMac-node/src/system/delay.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/system/eeprom.o: C:/workspace/eclipse/LoRaMac-node/src/system/eeprom.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/system/fifo.o: C:/workspace/eclipse/LoRaMac-node/src/system/fifo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/system/gpio.o: C:/workspace/eclipse/LoRaMac-node/src/system/gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/system/gps.o: C:/workspace/eclipse/LoRaMac-node/src/system/gps.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/system/i2c.o: C:/workspace/eclipse/LoRaMac-node/src/system/i2c.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/system/timer.o: C:/workspace/eclipse/LoRaMac-node/src/system/timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/system/uart.o: C:/workspace/eclipse/LoRaMac-node/src/system/uart.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


