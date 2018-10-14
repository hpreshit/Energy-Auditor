################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include protocol/bluetooth/lib/EFR32BG13P/GCC/subdir.mk
-include platform/emlib/src/subdir.mk
-include platform/emdrv/sleep/src/subdir.mk
-include platform/Device/SiliconLabs/EFR32BG13P/Source/GCC/subdir.mk
-include platform/Device/SiliconLabs/EFR32BG13P/Source/subdir.mk
-include hardware/kit/common/drivers/subdir.mk
-include hardware/kit/common/bsp/subdir.mk
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
all: Current-Sensor-BLE-Server.axf

# Tool invocations
Current-Sensor-BLE-Server.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\efr32bg13p632f512gm48.ld" -Xlinker --gc-sections -Xlinker -Map="Current-Sensor-BLE-Server.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o Current-Sensor-BLE-Server.axf -Wl,--start-group "./application_properties.o" "./gatt_db.o" "./init_app.o" "./init_board.o" "./init_mcu.o" "./main.o" "./pti.o" "./hardware/kit/common/bsp/bsp_stk.o" "./hardware/kit/common/drivers/mx25flash_spi.o" "./platform/Device/SiliconLabs/EFR32BG13P/Source/GCC/startup_efr32bg13p.o" "./platform/Device/SiliconLabs/EFR32BG13P/Source/system_efr32bg13p.o" "./platform/emdrv/sleep/src/sleep.o" "./platform/emlib/src/em_adc.o" "./platform/emlib/src/em_cmu.o" "./platform/emlib/src/em_core.o" "./platform/emlib/src/em_cryotimer.o" "./platform/emlib/src/em_crypto.o" "./platform/emlib/src/em_emu.o" "./platform/emlib/src/em_gpio.o" "./platform/emlib/src/em_i2c.o" "./platform/emlib/src/em_msc.o" "./platform/emlib/src/em_rmu.o" "./platform/emlib/src/em_rtcc.o" "./platform/emlib/src/em_system.o" "./platform/emlib/src/em_timer.o" "./platform/emlib/src/em_usart.o" "./src/adc0.o" "./src/cmu.o" "C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\radio\rail_lib\autogen\librail_release\librail_efr32xg13_gcc_release.a" "C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\protocol\bluetooth\lib\EFR32BG13P\GCC\libpsstore.a" "C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\protocol\bluetooth\lib\EFR32BG13P\GCC\binapploader.o" "C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\protocol\bluetooth\lib\EFR32BG13P\GCC\libbluetooth.a" "C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\protocol\bluetooth\lib\EFR32BG13P\GCC\libmbedtls.a" -lm -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: Current-Sensor-BLE-Server.hex'
	arm-none-eabi-objcopy -O ihex "Current-Sensor-BLE-Server.axf" "Current-Sensor-BLE-Server.hex"
	@echo ' '

	@echo 'Building bin file: Current-Sensor-BLE-Server.bin'
	arm-none-eabi-objcopy -O binary "Current-Sensor-BLE-Server.axf" "Current-Sensor-BLE-Server.bin"
	@echo ' '

	@echo 'Building s37 file: Current-Sensor-BLE-Server.s37'
	arm-none-eabi-objcopy -O srec "Current-Sensor-BLE-Server.axf" "Current-Sensor-BLE-Server.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "Current-Sensor-BLE-Server.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Current-Sensor-BLE-Server.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
