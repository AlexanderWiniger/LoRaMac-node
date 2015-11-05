################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/workspace/eclipse/LoRaMac-node/src/mac/LoRaMac.c \
C:/workspace/eclipse/LoRaMac-node/src/mac/LoRaMacCrypto.c 

OBJS += \
./src/mac/LoRaMac.o \
./src/mac/LoRaMacCrypto.o 

C_DEPS += \
./src/mac/LoRaMac.d \
./src/mac/LoRaMacCrypto.d 


# Each subdirectory must supply rules for building sources it contributes
src/mac/LoRaMac.o: C:/workspace/eclipse/LoRaMac-node/src/mac/LoRaMac.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis/utilities -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/mac/LoRaMacCrypto.o: C:/workspace/eclipse/LoRaMac-node/src/mac/LoRaMacCrypto.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis/utilities -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


