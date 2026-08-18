# Build the demo against the FreeRTOS POSIX (Linux) port so it runs on a PC.
#
# Point FREERTOS_KERNEL_PATH at a clone of https://github.com/FreeRTOS/FreeRTOS-Kernel
#   git clone --depth 1 https://github.com/FreeRTOS/FreeRTOS-Kernel.git
#   make FREERTOS_KERNEL_PATH=./FreeRTOS-Kernel run
#
# On an STM32 you would instead let STM32CubeIDE enable FreeRTOS (CMSIS-RTOS)
# and add main.c's tasks. See README.md.

FREERTOS_KERNEL_PATH ?= ./FreeRTOS-Kernel
PORT = $(FREERTOS_KERNEL_PATH)/portable/ThirdParty/GCC/Posix

CC = gcc
CFLAGS = -Wall -O2 -g -I. -I$(FREERTOS_KERNEL_PATH)/include -I$(PORT) -I$(PORT)/utils

KERNEL_SRCS = \
	$(FREERTOS_KERNEL_PATH)/tasks.c \
	$(FREERTOS_KERNEL_PATH)/queue.c \
	$(FREERTOS_KERNEL_PATH)/list.c \
	$(FREERTOS_KERNEL_PATH)/timers.c \
	$(FREERTOS_KERNEL_PATH)/event_groups.c \
	$(FREERTOS_KERNEL_PATH)/portable/MemMang/heap_4.c \
	$(PORT)/port.c \
	$(PORT)/utils/wait_for_event.c

SRCS = main.c $(KERNEL_SRCS)
TARGET = rtos_demo

.PHONY: all run clean
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) -lpthread

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
