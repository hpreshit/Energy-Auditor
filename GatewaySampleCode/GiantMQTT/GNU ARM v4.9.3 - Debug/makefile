################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/wrapper/subdir.mk
-include src/mqtt/subdir.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include Drivers/subdir.mk
-include CMSIS/EFM32GG/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: GiantMQTT.axf

# Tool invocations
GiantMQTT.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "GiantMQTT.ld" -Xlinker --gc-sections -Xlinker -Map="GiantMQTT.map" --specs=nano.specs -o GiantMQTT.axf "./CMSIS/EFM32GG/startup_efm32gg.o" "./CMSIS/EFM32GG/system_efm32gg.o" "./Drivers/dmactrl.o" "./Drivers/dmadrv.o" "./Drivers/gpiointerrupt.o" "./Drivers/rtcdriver.o" "./Drivers/uartdrv.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_dma.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_leuart.o" "./emlib/em_rtc.o" "./emlib/em_system.o" "./emlib/em_usart.o" "./src/main.o" "./src/mqtt/MQTTClient.o" "./src/mqtt/MQTTConnectClient.o" "./src/mqtt/MQTTConnectServer.o" "./src/mqtt/MQTTDeserializePublish.o" "./src/mqtt/MQTTPacket.o" "./src/mqtt/MQTTSerializePublish.o" "./src/mqtt/MQTTSubscribeClient.o" "./src/mqtt/MQTTSubscribeServer.o" "./src/mqtt/MQTTUnsubscribeClient.o" "./src/mqtt/MQTTUnsubscribeServer.o" "./src/wrapper/MQTTOsWrapper.o" 
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: GiantMQTT.hex'
	arm-none-eabi-objcopy -O ihex "GiantMQTT.axf" "GiantMQTT.hex"
	@echo ' '

	@echo 'Building bin file: GiantMQTT.bin'
	arm-none-eabi-objcopy -O binary "GiantMQTT.axf" "GiantMQTT.bin"
	@echo ' '

	@echo 'Building s37 file: GiantMQTT.s37'
	arm-none-eabi-objcopy -O srec "GiantMQTT.axf" "GiantMQTT.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "GiantMQTT.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) GiantMQTT.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
