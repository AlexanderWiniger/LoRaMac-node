################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/adc-board.c \
C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/board.c \
C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/gpio-board.c \
C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/i2c-board.c \
C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/rtc-board.c \
C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/spi-board.c \
C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/sx1276-board.c \
C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/timer-board.c \
C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/uart-board.c 

OBJS += \
./src/boards/FRDM-K22F/adc-board.o \
./src/boards/FRDM-K22F/board.o \
./src/boards/FRDM-K22F/gpio-board.o \
./src/boards/FRDM-K22F/i2c-board.o \
./src/boards/FRDM-K22F/rtc-board.o \
./src/boards/FRDM-K22F/spi-board.o \
./src/boards/FRDM-K22F/sx1276-board.o \
./src/boards/FRDM-K22F/timer-board.o \
./src/boards/FRDM-K22F/uart-board.o 

C_DEPS += \
./src/boards/FRDM-K22F/adc-board.d \
./src/boards/FRDM-K22F/board.d \
./src/boards/FRDM-K22F/gpio-board.d \
./src/boards/FRDM-K22F/i2c-board.d \
./src/boards/FRDM-K22F/rtc-board.d \
./src/boards/FRDM-K22F/spi-board.d \
./src/boards/FRDM-K22F/sx1276-board.d \
./src/boards/FRDM-K22F/timer-board.d \
./src/boards/FRDM-K22F/uart-board.d 


# Each subdirectory must supply rules for building sources it contributes
src/boards/FRDM-K22F/adc-board.o: C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/adc-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/FRDM-K22F/board.o: C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/FRDM-K22F/gpio-board.o: C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/gpio-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/FRDM-K22F/i2c-board.o: C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/i2c-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/FRDM-K22F/rtc-board.o: C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/rtc-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/FRDM-K22F/spi-board.o: C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/spi-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/FRDM-K22F/sx1276-board.o: C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/sx1276-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/FRDM-K22F/timer-board.o: C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/timer-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/FRDM-K22F/uart-board.o: C:/workspace/eclipse/LoRaMac-node/src/boards/FRDM-K22F/uart-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


