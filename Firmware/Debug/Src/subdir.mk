################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/adc.c \
../Src/butled.c \
../Src/clock.c \
../Src/eeprom.c \
../Src/i2c.c \
../Src/lfo.c \
../Src/lfo_sync.c \
../Src/main.c \
../Src/midi.c \
../Src/pwm.c \
../Src/stm32f0xx_it.c \
../Src/synth.c \
../Src/synth_lut.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/system_stm32f0xx.c \
../Src/tick.c 

OBJS += \
./Src/adc.o \
./Src/butled.o \
./Src/clock.o \
./Src/eeprom.o \
./Src/i2c.o \
./Src/lfo.o \
./Src/lfo_sync.o \
./Src/main.o \
./Src/midi.o \
./Src/pwm.o \
./Src/stm32f0xx_it.o \
./Src/synth.o \
./Src/synth_lut.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/system_stm32f0xx.o \
./Src/tick.o 

C_DEPS += \
./Src/adc.d \
./Src/butled.d \
./Src/clock.d \
./Src/eeprom.d \
./Src/i2c.d \
./Src/lfo.d \
./Src/lfo_sync.d \
./Src/main.d \
./Src/midi.d \
./Src/pwm.d \
./Src/stm32f0xx_it.d \
./Src/synth.d \
./Src/synth_lut.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/system_stm32f0xx.d \
./Src/tick.d 


# Each subdirectory must supply rules for building sources it contributes
Src/adc.o: ../Src/adc.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/adc.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/butled.o: ../Src/butled.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/butled.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/clock.o: ../Src/clock.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/clock.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/eeprom.o: ../Src/eeprom.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/eeprom.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/i2c.o: ../Src/i2c.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/i2c.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/lfo.o: ../Src/lfo.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/lfo.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/lfo_sync.o: ../Src/lfo_sync.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/lfo_sync.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/main.o: ../Src/main.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/main.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/midi.o: ../Src/midi.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/midi.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/pwm.o: ../Src/pwm.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/pwm.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/stm32f0xx_it.o: ../Src/stm32f0xx_it.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/stm32f0xx_it.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/synth.o: ../Src/synth.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/synth.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/synth_lut.o: ../Src/synth_lut.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/synth_lut.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/syscalls.o: ../Src/syscalls.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/syscalls.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/sysmem.o: ../Src/sysmem.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/sysmem.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/system_stm32f0xx.o: ../Src/system_stm32f0xx.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/system_stm32f0xx.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/tick.o: ../Src/tick.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F030C8Tx -c -I../Inc -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Include" -I"E:/Sx/Programming/stm-workspace/stm32f030-chord/CMSIS/Device/ST/STM32F0xx/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/tick.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

