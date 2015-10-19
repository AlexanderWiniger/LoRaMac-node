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
-include src/boards/mcu/startup/subdir.mk
-include src/boards/FRDM-K22F/subdir.mk
-include src/apps/LoRaMac/classA/FRDM-K22F/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: FRDM-K22F_LoRaMac-ClassA.elf secondary-outputs

# Tool invocations
FRDM-K22F_LoRaMac-ClassA.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -T "C:/dev/Freescale/KSDK_1.3.0/platform/devices/MK22F51212/linker/gcc/MK22FN512xxx12_flash.ld" -Xlinker --gc-sections -Wl,-Map,"FRDM-K22F_LoRaMac-ClassA.map" -Xlinker -z  -Xlinker muldefs   --specs=nano.specs          -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -Xlinker -static  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000 -o "FRDM-K22F_LoRaMac-ClassA.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) FRDM-K22F_LoRaMac-ClassA.elf
	-@echo ' '

secondary-outputs:

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
