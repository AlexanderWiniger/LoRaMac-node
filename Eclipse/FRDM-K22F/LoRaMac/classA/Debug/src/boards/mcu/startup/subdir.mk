################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/dev/Freescale/KSDK_1.3.0/platform/devices/startup.c \
C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup/system_MK22F51212.c 

S_UPPER_SRCS += \
C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup/gcc/startup_MK22F51212.S 

OBJS += \
./src/boards/mcu/startup/startup.o \
./src/boards/mcu/startup/startup_MK22F51212.o \
./src/boards/mcu/startup/system_MK22F51212.o 

C_DEPS += \
./src/boards/mcu/startup/startup.d \
./src/boards/mcu/startup/system_MK22F51212.d 

S_UPPER_DEPS += \
./src/boards/mcu/startup/startup_MK22F51212.d 


# Each subdirectory must supply rules for building sources it contributes
src/boards/mcu/startup/startup.o: C:/dev/Freescale/KSDK_1.3.0/platform/devices/startup.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/mcu/startup/startup_MK22F51212.o: C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup/gcc/startup_MK22F51212.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -x assembler-with-cpp -DDEBUG -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/mcu/startup/system_MK22F51212.o: C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup/system_MK22F51212.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_DEBUGGER -DDEBUG -DFREEDOM -DFRDM_K22F -DCPU_MK22FN512VLH12 -I../../../../../src/system -I../../../../../src/system/crypto -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/mac -I../../../../../src/peripherals -I../../../../../src/boards/FRDM-K22F -I../../../../../src/boards/mcu/kinetis -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/startup" -I"C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/include" -I"C:/dev/Freescale/KSDK_1.3.0/platform/hal/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/drivers/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/system/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/utilities/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/osa/inc" -I"C:/dev/Freescale/KSDK_1.3.0/platform/CMSIS/Include" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


