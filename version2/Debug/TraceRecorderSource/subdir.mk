################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TraceRecorderSource/trcAssert.c \
../TraceRecorderSource/trcCounter.c \
../TraceRecorderSource/trcDependency.c \
../TraceRecorderSource/trcDiagnostics.c \
../TraceRecorderSource/trcEntryTable.c \
../TraceRecorderSource/trcError.c \
../TraceRecorderSource/trcEvent.c \
../TraceRecorderSource/trcEventBuffer.c \
../TraceRecorderSource/trcExtension.c \
../TraceRecorderSource/trcHardwarePort.c \
../TraceRecorderSource/trcHeap.c \
../TraceRecorderSource/trcISR.c \
../TraceRecorderSource/trcInternalEventBuffer.c \
../TraceRecorderSource/trcInterval.c \
../TraceRecorderSource/trcMultiCoreEventBuffer.c \
../TraceRecorderSource/trcObject.c \
../TraceRecorderSource/trcPrint.c \
../TraceRecorderSource/trcRunnable.c \
../TraceRecorderSource/trcStackMonitor.c \
../TraceRecorderSource/trcStateMachine.c \
../TraceRecorderSource/trcStaticBuffer.c \
../TraceRecorderSource/trcStreamingRecorder.c \
../TraceRecorderSource/trcString.c \
../TraceRecorderSource/trcTask.c \
../TraceRecorderSource/trcTaskMonitor.c \
../TraceRecorderSource/trcTimestamp.c 

OBJS += \
./TraceRecorderSource/trcAssert.o \
./TraceRecorderSource/trcCounter.o \
./TraceRecorderSource/trcDependency.o \
./TraceRecorderSource/trcDiagnostics.o \
./TraceRecorderSource/trcEntryTable.o \
./TraceRecorderSource/trcError.o \
./TraceRecorderSource/trcEvent.o \
./TraceRecorderSource/trcEventBuffer.o \
./TraceRecorderSource/trcExtension.o \
./TraceRecorderSource/trcHardwarePort.o \
./TraceRecorderSource/trcHeap.o \
./TraceRecorderSource/trcISR.o \
./TraceRecorderSource/trcInternalEventBuffer.o \
./TraceRecorderSource/trcInterval.o \
./TraceRecorderSource/trcMultiCoreEventBuffer.o \
./TraceRecorderSource/trcObject.o \
./TraceRecorderSource/trcPrint.o \
./TraceRecorderSource/trcRunnable.o \
./TraceRecorderSource/trcStackMonitor.o \
./TraceRecorderSource/trcStateMachine.o \
./TraceRecorderSource/trcStaticBuffer.o \
./TraceRecorderSource/trcStreamingRecorder.o \
./TraceRecorderSource/trcString.o \
./TraceRecorderSource/trcTask.o \
./TraceRecorderSource/trcTaskMonitor.o \
./TraceRecorderSource/trcTimestamp.o 

C_DEPS += \
./TraceRecorderSource/trcAssert.d \
./TraceRecorderSource/trcCounter.d \
./TraceRecorderSource/trcDependency.d \
./TraceRecorderSource/trcDiagnostics.d \
./TraceRecorderSource/trcEntryTable.d \
./TraceRecorderSource/trcError.d \
./TraceRecorderSource/trcEvent.d \
./TraceRecorderSource/trcEventBuffer.d \
./TraceRecorderSource/trcExtension.d \
./TraceRecorderSource/trcHardwarePort.d \
./TraceRecorderSource/trcHeap.d \
./TraceRecorderSource/trcISR.d \
./TraceRecorderSource/trcInternalEventBuffer.d \
./TraceRecorderSource/trcInterval.d \
./TraceRecorderSource/trcMultiCoreEventBuffer.d \
./TraceRecorderSource/trcObject.d \
./TraceRecorderSource/trcPrint.d \
./TraceRecorderSource/trcRunnable.d \
./TraceRecorderSource/trcStackMonitor.d \
./TraceRecorderSource/trcStateMachine.d \
./TraceRecorderSource/trcStaticBuffer.d \
./TraceRecorderSource/trcStreamingRecorder.d \
./TraceRecorderSource/trcString.d \
./TraceRecorderSource/trcTask.d \
./TraceRecorderSource/trcTaskMonitor.d \
./TraceRecorderSource/trcTimestamp.d 


# Each subdirectory must supply rules for building sources it contributes
TraceRecorderSource/%.o TraceRecorderSource/%.su TraceRecorderSource/%.cyclo: ../TraceRecorderSource/%.c TraceRecorderSource/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"D:/Nam_4/KI_II/Thiet_ke_so_va_lap_trinh_nhung/FC_freeRtosv2/version2/TraceRecorderSource/include" -I"D:/Nam_4/KI_II/Thiet_ke_so_va_lap_trinh_nhung/FC_freeRtosv2/version2/TraceRecorderSource/config" -I"D:/Nam_4/KI_II/Thiet_ke_so_va_lap_trinh_nhung/FC_freeRtosv2/version2/TraceRecorderSource/kernelports/FreeRTOS/include" -I"D:/Nam_4/KI_II/Thiet_ke_so_va_lap_trinh_nhung/FC_freeRtosv2/version2/TraceRecorderSource/kernelports/FreeRTOS/config" -I"D:/Nam_4/KI_II/Thiet_ke_so_va_lap_trinh_nhung/FC_freeRtosv2/version2/TraceRecorderSource/streamports/RingBuffer/config" -I"D:/Nam_4/KI_II/Thiet_ke_so_va_lap_trinh_nhung/FC_freeRtosv2/version2/TraceRecorderSource/streamports/RingBuffer/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TraceRecorderSource

clean-TraceRecorderSource:
	-$(RM) ./TraceRecorderSource/trcAssert.cyclo ./TraceRecorderSource/trcAssert.d ./TraceRecorderSource/trcAssert.o ./TraceRecorderSource/trcAssert.su ./TraceRecorderSource/trcCounter.cyclo ./TraceRecorderSource/trcCounter.d ./TraceRecorderSource/trcCounter.o ./TraceRecorderSource/trcCounter.su ./TraceRecorderSource/trcDependency.cyclo ./TraceRecorderSource/trcDependency.d ./TraceRecorderSource/trcDependency.o ./TraceRecorderSource/trcDependency.su ./TraceRecorderSource/trcDiagnostics.cyclo ./TraceRecorderSource/trcDiagnostics.d ./TraceRecorderSource/trcDiagnostics.o ./TraceRecorderSource/trcDiagnostics.su ./TraceRecorderSource/trcEntryTable.cyclo ./TraceRecorderSource/trcEntryTable.d ./TraceRecorderSource/trcEntryTable.o ./TraceRecorderSource/trcEntryTable.su ./TraceRecorderSource/trcError.cyclo ./TraceRecorderSource/trcError.d ./TraceRecorderSource/trcError.o ./TraceRecorderSource/trcError.su ./TraceRecorderSource/trcEvent.cyclo ./TraceRecorderSource/trcEvent.d ./TraceRecorderSource/trcEvent.o ./TraceRecorderSource/trcEvent.su ./TraceRecorderSource/trcEventBuffer.cyclo ./TraceRecorderSource/trcEventBuffer.d ./TraceRecorderSource/trcEventBuffer.o ./TraceRecorderSource/trcEventBuffer.su ./TraceRecorderSource/trcExtension.cyclo ./TraceRecorderSource/trcExtension.d ./TraceRecorderSource/trcExtension.o ./TraceRecorderSource/trcExtension.su ./TraceRecorderSource/trcHardwarePort.cyclo ./TraceRecorderSource/trcHardwarePort.d ./TraceRecorderSource/trcHardwarePort.o ./TraceRecorderSource/trcHardwarePort.su ./TraceRecorderSource/trcHeap.cyclo ./TraceRecorderSource/trcHeap.d ./TraceRecorderSource/trcHeap.o ./TraceRecorderSource/trcHeap.su ./TraceRecorderSource/trcISR.cyclo ./TraceRecorderSource/trcISR.d ./TraceRecorderSource/trcISR.o ./TraceRecorderSource/trcISR.su ./TraceRecorderSource/trcInternalEventBuffer.cyclo ./TraceRecorderSource/trcInternalEventBuffer.d ./TraceRecorderSource/trcInternalEventBuffer.o ./TraceRecorderSource/trcInternalEventBuffer.su ./TraceRecorderSource/trcInterval.cyclo ./TraceRecorderSource/trcInterval.d ./TraceRecorderSource/trcInterval.o ./TraceRecorderSource/trcInterval.su ./TraceRecorderSource/trcMultiCoreEventBuffer.cyclo ./TraceRecorderSource/trcMultiCoreEventBuffer.d ./TraceRecorderSource/trcMultiCoreEventBuffer.o ./TraceRecorderSource/trcMultiCoreEventBuffer.su ./TraceRecorderSource/trcObject.cyclo ./TraceRecorderSource/trcObject.d ./TraceRecorderSource/trcObject.o ./TraceRecorderSource/trcObject.su ./TraceRecorderSource/trcPrint.cyclo ./TraceRecorderSource/trcPrint.d ./TraceRecorderSource/trcPrint.o ./TraceRecorderSource/trcPrint.su ./TraceRecorderSource/trcRunnable.cyclo ./TraceRecorderSource/trcRunnable.d ./TraceRecorderSource/trcRunnable.o ./TraceRecorderSource/trcRunnable.su ./TraceRecorderSource/trcStackMonitor.cyclo ./TraceRecorderSource/trcStackMonitor.d ./TraceRecorderSource/trcStackMonitor.o ./TraceRecorderSource/trcStackMonitor.su ./TraceRecorderSource/trcStateMachine.cyclo ./TraceRecorderSource/trcStateMachine.d ./TraceRecorderSource/trcStateMachine.o ./TraceRecorderSource/trcStateMachine.su ./TraceRecorderSource/trcStaticBuffer.cyclo ./TraceRecorderSource/trcStaticBuffer.d ./TraceRecorderSource/trcStaticBuffer.o ./TraceRecorderSource/trcStaticBuffer.su ./TraceRecorderSource/trcStreamingRecorder.cyclo ./TraceRecorderSource/trcStreamingRecorder.d ./TraceRecorderSource/trcStreamingRecorder.o ./TraceRecorderSource/trcStreamingRecorder.su ./TraceRecorderSource/trcString.cyclo ./TraceRecorderSource/trcString.d ./TraceRecorderSource/trcString.o ./TraceRecorderSource/trcString.su ./TraceRecorderSource/trcTask.cyclo ./TraceRecorderSource/trcTask.d ./TraceRecorderSource/trcTask.o ./TraceRecorderSource/trcTask.su ./TraceRecorderSource/trcTaskMonitor.cyclo ./TraceRecorderSource/trcTaskMonitor.d ./TraceRecorderSource/trcTaskMonitor.o ./TraceRecorderSource/trcTaskMonitor.su ./TraceRecorderSource/trcTimestamp.cyclo ./TraceRecorderSource/trcTimestamp.d ./TraceRecorderSource/trcTimestamp.o ./TraceRecorderSource/trcTimestamp.su

.PHONY: clean-TraceRecorderSource

