################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/system/crypto/subdir.mk
-include src/system/subdir.mk
-include src/radio/sx1276/subdir.mk
-include src/mac/subdir.mk
-include src/free-rtos/src/subdir.mk
-include src/free-rtos/port/subdir.mk
-include src/boards/tinyK20/subdir.mk
-include src/boards/mcu/kinetis/utilities/subdir.mk
-include src/boards/mcu/kinetis/k20d/startup/subdir.mk
-include src/apps/LoRaMesh/rtos/tinyK20/subdir.mk
-include src/apps/LoRaMesh/rtos/Shell_App/subdir.mk
-include src/apps/LoRaMesh/rtos/LoRaStack/subdir.mk
-include src/apps/LoRaMesh/rtos/LoRaMesh_App/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_SIZE += \
tinyK20_LoRaMesh-rtos.siz \


# All Target
all: tinyK20_LoRaMesh-rtos.elf secondary-outputs

# Tool invocations
tinyK20_LoRaMesh-rtos.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -T ../../../../../src/boards/mcu/kinetis/k20d/linker/MK20D128xxx5_flash.ld -Xlinker --gc-sections -Wl,-Map,"tinyK20_LoRaMesh-rtos.map" -specs=nosys.specs -o "tinyK20_LoRaMesh-rtos.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

tinyK20_LoRaMesh-rtos.siz: tinyK20_LoRaMesh-rtos.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=sysv "tinyK20_LoRaMesh-rtos.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(S_UPPER_DEPS) tinyK20_LoRaMesh-rtos.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
