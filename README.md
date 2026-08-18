# FreeRTOS — Tasks, Queue & Mutex

A FreeRTOS application demonstrating the core real-time-OS primitives: two
producer tasks feed sensor readings through a **queue** to a consumer task, and
both producers update a **shared statistics block** protected by a **mutex**.

```
  [sensorA task] --\
                    >--(xQueue, depth 8)--> [logger task]
  [sensorB task] --/
        \______ both update shared stats under a mutex ______/
```

## Concepts demonstrated

- `xTaskCreate` / `vTaskStartScheduler` — creating tasks and starting the
  preemptive scheduler.
- `xQueueSend` / `xQueueReceive` — blocking, thread-safe message passing.
- `xSemaphoreCreateMutex` + `xSemaphoreTake/Give` — a critical section with
  **priority inheritance** to bound priority inversion.
- `vTaskDelay` — yields the CPU (unlike a busy-wait), so lower-priority work and
  the idle task still run.

## Build & run

**A. On a PC via the FreeRTOS POSIX port (Linux / WSL):**

```bash
git clone --depth 1 https://github.com/FreeRTOS/FreeRTOS-Kernel.git
make FREERTOS_KERNEL_PATH=./FreeRTOS-Kernel run
```

Prints interleaved sensor messages and periodic stats snapshots, ending with
`processed 20 messages -- demo complete.`

> The POSIX port targets Linux (it uses pthreads and POSIX signals). On Windows,
> run it inside **WSL** (Windows Subsystem for Linux) or any Linux environment.

**B. On an STM32 (STM32CubeIDE):** create a project for your STM32, enable
**FreeRTOS (CMSIS-RTOS v2)** in CubeMX, and drop the tasks from `main.c` in.
Replace `printf` with your UART transmit; CubeMX generates `FreeRTOSConfig.h`.

## Design note

The queue already serialises the sensor messages safely. The **mutex** protects
a *separate* shared `stats` struct that both producer tasks write — without it, a
context switch mid-update is a classic race condition. A FreeRTOS mutex also
provides **priority inheritance**, which a plain binary semaphore does not.

## Files

```
main.c              tasks, queue, mutex, and the application logic
FreeRTOSConfig.h    kernel config (tick rate, heap, priorities, hooks)
Makefile            builds against the FreeRTOS POSIX port
```

## License

Released under the MIT License — free to use and adapt.
