################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hardware/kit/common/drivers/mx25flash_spi.c 

OBJS += \
./hardware/kit/common/drivers/mx25flash_spi.o 

C_DEPS += \
./hardware/kit/common/drivers/mx25flash_spi.d 


# Each subdirectory must supply rules for building sources it contributes
hardware/kit/common/drivers/mx25flash_spi.o: ../hardware/kit/common/drivers/mx25flash_spi.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DEFR32BG13P632F512GM48=1' '-D__HEAP_SIZE=0xD00' '-D__STACK_SIZE=0x800' '-DHAL_CONFIG=1' '-D__StackLimit=0x20000000' -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\emlib\inc" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\hardware\kit\EFR32BG13_BRD4104A\config" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\Device\SiliconLabs\EFR32BG13P\Include" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\hardware\kit\common\halconfig" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\hardware\kit\common\bsp" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\emlib\src" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\protocol\bluetooth\ble_stack\inc\soc" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\CMSIS\Include" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\hardware\kit\common\drivers" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\Device\SiliconLabs\EFR32BG13P\Source\GCC" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\emdrv\sleep\src" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\radio\rail_lib\common" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\Device\SiliconLabs\EFR32BG13P\Source" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\protocol\bluetooth\ble_stack\inc\common" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\emdrv\gpiointerrupt\inc" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\halconfig\inc\hal-config" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\emdrv\common\inc" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\emdrv\sleep\inc" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\bootloader\api" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\emdrv\uartdrv\inc" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\radio\rail_lib\chip\efr32" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\app\bluetooth\common\stack_bridge" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\platform\bootloader" -I"C:\Users\Gunj Manseta\SimplicityStudio\SampleCodeWorkspace\Current-Sensor-BLE-Server\src" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"hardware/kit/common/drivers/mx25flash_spi.d" -MT"hardware/kit/common/drivers/mx25flash_spi.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


