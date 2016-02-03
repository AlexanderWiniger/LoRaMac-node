################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/workspace/kds/LoRaMac-node/src/boards/mcu/kinetis/k20d/startup/startup.c \
C:/workspace/kds/LoRaMac-node/src/boards/mcu/kinetis/k20d/startup/system_MK20D5.c \
C:/workspace/kds/LoRaMac-node/src/boards/mcu/kinetis/k20d/startup/vectors.c 

OBJS += \
./src/boards/mcu/kinetis/k20d/startup/startup.o \
./src/boards/mcu/kinetis/k20d/startup/system_MK20D5.o \
./src/boards/mcu/kinetis/k20d/startup/vectors.o 

C_DEPS += \
./src/boards/mcu/kinetis/k20d/startup/startup.d \
./src/boards/mcu/kinetis/k20d/startup/system_MK20D5.d \
./src/boards/mcu/kinetis/k20d/startup/vectors.d 


# Each subdirectory must supply rules for building sources it contributes
src/boards/mcu/kinetis/k20d/startup/startup.o: C:/workspace/kds/LoRaMac-node/src/boards/mcu/kinetis/k20d/startup/startup.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_A -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/mcu/kinetis/k20d/startup/system_MK20D5.o: C:/workspace/kds/LoRaMac-node/src/boards/mcu/kinetis/k20d/startup/system_MK20D5.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_A -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/mcu/kinetis/k20d/startup/vectors.o: C:/workspace/kds/LoRaMac-node/src/boards/mcu/kinetis/k20d/startup/vectors.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_A -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


