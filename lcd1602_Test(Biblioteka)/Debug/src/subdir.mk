################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/delay.c \
../src/lcd16x2.c \
../src/main.c \
../src/syscalls.c \
../src/system_stm32f10x.c 

OBJS += \
./src/delay.o \
./src/lcd16x2.o \
./src/main.o \
./src/syscalls.o \
./src/system_stm32f10x.o 

C_DEPS += \
./src/delay.d \
./src/lcd16x2.d \
./src/main.d \
./src/syscalls.d \
./src/system_stm32f10x.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F1 -DSTM32F103RBTx -DNUCLEO_F103RB -DDEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -I"E:/S4W/Workspace/lcd1602_Test/Utilities/STM32F1xx-Nucleo" -I"E:/S4W/Workspace/lcd1602_Test/StdPeriph_Driver/inc" -I"E:/S4W/Workspace/lcd1602_Test/inc" -I"E:/S4W/Workspace/lcd1602_Test/CMSIS/device" -I"E:/S4W/Workspace/lcd1602_Test/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


