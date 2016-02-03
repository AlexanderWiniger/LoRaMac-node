################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/workspace/kds/LoRaMac-node/src/free-rtos/port/port.c 

S_UPPER_SRCS += \
C:/workspace/kds/LoRaMac-node/src/free-rtos/port/portasm.S 

OBJS += \
./src/free-rtos/port/port.o \
./src/free-rtos/port/portasm.o 

C_DEPS += \
./src/free-rtos/port/port.d 

S_UPPER_DEPS += \
./src/free-rtos/port/portasm.d 


# Each subdirectory must supply rules for building sources it contributes
src/free-rtos/port/port.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/port/port.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_A -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/free-rtos/port/portasm.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/port/portasm.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -x assembler-with-cpp -DDEBUG -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


