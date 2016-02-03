################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/adc-board.c \
C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/board.c \
C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/gpio-board.c \
C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/gps-board.c \
C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/i2c-board.c \
C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/rtc-board.c \
C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/spi-board.c \
C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/sx1276-board.c \
C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/timer-board.c \
C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/uart-board.c 

OBJS += \
./src/boards/tinyK20/adc-board.o \
./src/boards/tinyK20/board.o \
./src/boards/tinyK20/gpio-board.o \
./src/boards/tinyK20/gps-board.o \
./src/boards/tinyK20/i2c-board.o \
./src/boards/tinyK20/rtc-board.o \
./src/boards/tinyK20/spi-board.o \
./src/boards/tinyK20/sx1276-board.o \
./src/boards/tinyK20/timer-board.o \
./src/boards/tinyK20/uart-board.o 

C_DEPS += \
./src/boards/tinyK20/adc-board.d \
./src/boards/tinyK20/board.d \
./src/boards/tinyK20/gpio-board.d \
./src/boards/tinyK20/gps-board.d \
./src/boards/tinyK20/i2c-board.d \
./src/boards/tinyK20/rtc-board.d \
./src/boards/tinyK20/spi-board.d \
./src/boards/tinyK20/sx1276-board.d \
./src/boards/tinyK20/timer-board.d \
./src/boards/tinyK20/uart-board.d 


# Each subdirectory must supply rules for building sources it contributes
src/boards/tinyK20/adc-board.o: C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/adc-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_B -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/tinyK20/board.o: C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_B -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/tinyK20/gpio-board.o: C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/gpio-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_B -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/tinyK20/gps-board.o: C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/gps-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_B -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/tinyK20/i2c-board.o: C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/i2c-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_B -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/tinyK20/rtc-board.o: C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/rtc-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_B -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/tinyK20/spi-board.o: C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/spi-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_B -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/tinyK20/sx1276-board.o: C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/sx1276-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_B -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/tinyK20/timer-board.o: C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/timer-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_B -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boards/tinyK20/uart-board.o: C:/workspace/kds/LoRaMac-node/src/boards/tinyK20/uart-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_B -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


