################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TraceRecorderSource/streamports/TCPIP/trcStreamPort.c 

OBJS += \
./TraceRecorderSource/streamports/TCPIP/trcStreamPort.o 

C_DEPS += \
./TraceRecorderSource/streamports/TCPIP/trcStreamPort.d 


# Each subdirectory must supply rules for building sources it contributes
TraceRecorderSource/streamports/TCPIP/%.o TraceRecorderSource/streamports/TCPIP/%.su TraceRecorderSource/streamports/TCPIP/%.cyclo: ../TraceRecorderSource/streamports/TCPIP/%.c TraceRecorderSource/streamports/TCPIP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"D:/Nam_4/KI_II/Thiet_ke_so_va_lap_trinh_nhung/FC_freeRtosv2/version2/TraceRecorderSource/include" -I"D:/Nam_4/KI_II/Thiet_ke_so_va_lap_trinh_nhung/FC_freeRtosv2/version2/TraceRecorderSource/config" -I"D:/Nam_4/KI_II/Thiet_ke_so_va_lap_trinh_nhung/FC_freeRtosv2/version2/TraceRecorderSource/kernelports/FreeRTOS/include" -I"D:/Nam_4/KI_II/Thiet_ke_so_va_lap_trinh_nhung/FC_freeRtosv2/version2/TraceRecorderSource/kernelports/FreeRTOS/config" -I"D:/Nam_4/KI_II/Thiet_ke_so_va_lap_trinh_nhung/FC_freeRtosv2/version2/TraceRecorderSource/streamports/RingBuffer/config" -I"D:/Nam_4/KI_II/Thiet_ke_so_va_lap_trinh_nhung/FC_freeRtosv2/version2/TraceRecorderSource/streamports/RingBuffer/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TraceRecorderSource-2f-streamports-2f-TCPIP

clean-TraceRecorderSource-2f-streamports-2f-TCPIP:
	-$(RM) ./TraceRecorderSource/streamports/TCPIP/trcStreamPort.cyclo ./TraceRecorderSource/streamports/TCPIP/trcStreamPort.d ./TraceRecorderSource/streamports/TCPIP/trcStreamPort.o ./TraceRecorderSource/streamports/TCPIP/trcStreamPort.su

.PHONY: clean-TraceRecorderSource-2f-streamports-2f-TCPIP

