/*
 * FreeRTOSConfig.h  --  Kernel configuration for the PC (POSIX) simulation.
 *
 * On a real STM32 target STM32CubeIDE generates this file for you; the values
 * that matter conceptually are the same: tick rate, heap size, priority
 * levels, and which API features are compiled in. Comments note what each
 * key setting controls.
 */
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configUSE_PREEMPTION            1   /* time-sliced preemptive scheduler */
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0
#define configTICK_RATE_HZ              1000              /* 1 ms tick */
#define configMINIMAL_STACK_SIZE        ( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE           ( ( size_t ) ( 64 * 1024 ) )
#define configMAX_TASK_NAME_LEN         16
#define configUSE_TRACE_FACILITY        0
#define configUSE_16_BIT_TICKS          0
#define configIDLE_SHOULD_YIELD         1
#define configUSE_MUTEXES               1   /* enables xSemaphoreCreateMutex   */
#define configUSE_RECURSIVE_MUTEXES     1
#define configUSE_COUNTING_SEMAPHORES   1
#define configQUEUE_REGISTRY_SIZE       8
#define configMAX_PRIORITIES            7
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configSUPPORT_STATIC_ALLOCATION  0

/* Hook functions */
#define configCHECK_FOR_STACK_OVERFLOW  1   /* calls vApplicationStackOverflowHook */
#define configUSE_MALLOC_FAILED_HOOK    1   /* calls vApplicationMallocFailedHook  */

/* Software timers */
#define configUSE_TIMERS                1
#define configTIMER_TASK_PRIORITY       ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH        10
#define configTIMER_TASK_STACK_DEPTH    ( configMINIMAL_STACK_SIZE * 2 )

/* Include the API functions this demo uses. */
#define INCLUDE_vTaskDelay              1
#define INCLUDE_vTaskDelayUntil         1
#define INCLUDE_vTaskPrioritySet        1
#define INCLUDE_uxTaskPriorityGet       1
#define INCLUDE_vTaskDelete             1
#define INCLUDE_vTaskSuspend            1
#define INCLUDE_xTaskGetSchedulerState  1
#define INCLUDE_xTaskGetIdleTaskHandle  1

#endif /* FREERTOS_CONFIG_H */
