################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/workspace/kds/LoRaMac-node/src/free-rtos/src/croutine.c \
C:/workspace/kds/LoRaMac-node/src/free-rtos/src/event_groups.c \
C:/workspace/kds/LoRaMac-node/src/free-rtos/src/heap_1.c \
C:/workspace/kds/LoRaMac-node/src/free-rtos/src/heap_2.c \
C:/workspace/kds/LoRaMac-node/src/free-rtos/src/heap_3.c \
C:/workspace/kds/LoRaMac-node/src/free-rtos/src/heap_4.c \
C:/workspace/kds/LoRaMac-node/src/free-rtos/src/heap_5.c \
C:/workspace/kds/LoRaMac-node/src/free-rtos/src/list.c \
C:/workspace/kds/LoRaMac-node/src/free-rtos/src/queue.c \
C:/workspace/kds/LoRaMac-node/src/free-rtos/src/tasks.c \
C:/workspace/kds/LoRaMac-node/src/free-rtos/src/timers.c 

OBJS += \
./src/free-rtos/src/croutine.o \
./src/free-rtos/src/event_groups.o \
./src/free-rtos/src/heap_1.o \
./src/free-rtos/src/heap_2.o \
./src/free-rtos/src/heap_3.o \
./src/free-rtos/src/heap_4.o \
./src/free-rtos/src/heap_5.o \
./src/free-rtos/src/list.o \
./src/free-rtos/src/queue.o \
./src/free-rtos/src/tasks.o \
./src/free-rtos/src/timers.o 

C_DEPS += \
./src/free-rtos/src/croutine.d \
./src/free-rtos/src/event_groups.d \
./src/free-rtos/src/heap_1.d \
./src/free-rtos/src/heap_2.d \
./src/free-rtos/src/heap_3.d \
./src/free-rtos/src/heap_4.d \
./src/free-rtos/src/heap_5.d \
./src/free-rtos/src/list.d \
./src/free-rtos/src/queue.d \
./src/free-rtos/src/tasks.d \
./src/free-rtos/src/timers.d 


# Each subdirectory must supply rules for building sources it contributes
src/free-rtos/src/croutine.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/src/croutine.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_E -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/free-rtos/src/event_groups.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/src/event_groups.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_E -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/free-rtos/src/heap_1.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/src/heap_1.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_E -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/free-rtos/src/heap_2.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/src/heap_2.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_E -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/free-rtos/src/heap_3.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/src/heap_3.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_E -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/free-rtos/src/heap_4.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/src/heap_4.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_E -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/free-rtos/src/heap_5.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/src/heap_5.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_E -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/free-rtos/src/list.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/src/list.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_E -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/free-rtos/src/queue.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/src/queue.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_E -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/free-rtos/src/tasks.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/src/tasks.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_E -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/free-rtos/src/timers.o: C:/workspace/kds/LoRaMac-node/src/free-rtos/src/timers.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DUSE_SHELL -DUSE_BAND_868 -DUSE_MODEM_LORA -DUSE_LORA_MESH -DUSE_DEBUGGER -DUSE_CUSTOM_UART_HAL -DUSE_FREE_RTOS -DSX1276_BOARD_EMBED -DNODE_E -I../../../../../src/apps/LoRaMesh/rtos/tinyK20 -I../../../../../src/apps/LoRaMesh/rtos/Shell_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaMesh_App -I../../../../../src/apps/LoRaMesh/rtos/LoRaStack -I../../../../../src/boards/tinyK20 -I../../../../../src/boards/mcu/kinetis/k20d -I../../../../../src/boards/mcu/kinetis/k20d/startup -I../../../../../src/boards/mcu/kinetis/utilities -I../../../../../src/free-rtos/config/tinyK20 -I../../../../../src/free-rtos/include -I../../../../../src/free-rtos/port -I../../../../../src/mac -I../../../../../src/radio -I../../../../../src/radio/sx1276 -I../../../../../src/system -I../../../../../src/system/crypto -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


