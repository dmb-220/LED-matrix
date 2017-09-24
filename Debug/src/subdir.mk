################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ds1307.c \
../src/i2c.c \
../src/i2cm.c \
../src/main.c \
../src/milli-timer.c \
../src/rda5807.c \
../src/systick-utils.c \
../src/usart.c 

OBJS += \
./src/ds1307.o \
./src/i2c.o \
./src/i2cm.o \
./src/main.o \
./src/milli-timer.o \
./src/rda5807.o \
./src/systick-utils.o \
./src/usart.o 

C_DEPS += \
./src/ds1307.d \
./src/i2c.d \
./src/i2cm.d \
./src/main.d \
./src/milli-timer.d \
./src/rda5807.d \
./src/systick-utils.d \
./src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=72000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


