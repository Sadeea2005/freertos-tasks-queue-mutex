/*
 * main.c  --  FreeRTOS: two tasks, a queue, and a mutex.
 * ---------------------------------------------------------------------------
 * A common RTOS pattern for decoupling producer tasks from a consumer:
 *
 *   [Sensor task A] --\
 *                      >--(xQueue)--> [Logger task C]
 *   [Sensor task B] --/
 *
 *   Both sensor tasks also update a SHARED statistics block. Because two
 *   tasks touch it, every access is wrapped in a MUTEX so an update can't be
 *   interrupted half-way (the classic race condition). The mutex also gives
 *   us priority inheritance, which prevents unbounded priority inversion.
 *
 * Concepts demonstrated:
 *   - Task creation & the scheduler (xTaskCreate, vTaskStartScheduler)
 *   - Blocking inter-task communication via a queue (xQueueSend/Receive)
 *   - Mutual exclusion on shared state via a mutex (xSemaphoreTake/Give)
 *   - Cooperative timing with vTaskDelay (yields the CPU, unlike a busy-wait)
 *
 * It builds two ways (see README):
 *   1. On your PC via the FreeRTOS POSIX port  -> prints to the terminal.
 *   2. On an STM32 via STM32CubeIDE + CMSIS-RTOS -> replace printf with UART.
 */
#include <stdio.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* ---- A message that travels through the queue --------------------------- */
typedef struct {
    uint8_t  source_id;   /* which sensor produced this        */
    uint32_t sample;      /* the "reading"                     */
    uint32_t tick;        /* timestamp (scheduler ticks)       */
} sensor_msg_t;

/* ---- Shared state guarded by a mutex ------------------------------------ */
typedef struct {
    uint32_t total_samples;
    uint32_t last_by_source[2];
} stats_t;

static stats_t          g_stats;
static QueueHandle_t    g_queue;
static SemaphoreHandle_t g_stats_mutex;

/* Producer: generate a reading, publish it to the queue, and update the
 * shared stats under the mutex. Two instances run with different IDs. */
static void sensor_task(void *arg) {
    uint8_t  id   = (uint8_t)(uintptr_t)arg;
    uint32_t seed = (id + 1u) * 7u;

    for (;;) {
        seed = seed * 1103515245u + 12345u;      /* tiny LCG "sensor" */
        sensor_msg_t msg = {
            .source_id = id,
            .sample    = (seed >> 16) & 0x3FFu,
            .tick      = (uint32_t)xTaskGetTickCount(),
        };

        /* Send to the logger. Block up to 100 ms if the queue is full. */
        xQueueSend(g_queue, &msg, pdMS_TO_TICKS(100));

        /* Critical section: only one task may touch g_stats at a time. */
        if (xSemaphoreTake(g_stats_mutex, portMAX_DELAY) == pdTRUE) {
            g_stats.total_samples++;
            g_stats.last_by_source[id] = msg.sample;
            xSemaphoreGive(g_stats_mutex);
        }

        vTaskDelay(pdMS_TO_TICKS(id == 0 ? 200 : 350));  /* different rates */
    }
}

/* Consumer: block on the queue, print each message, and every few messages
 * read the shared stats (again under the mutex) to show a consistent view. */
static void logger_task(void *arg) {
    (void)arg;
    sensor_msg_t msg;
    uint32_t received = 0;

    for (;;) {
        if (xQueueReceive(g_queue, &msg, portMAX_DELAY) == pdTRUE) {
            received++;
            printf("[logger] sensor=%u sample=%3u  @tick=%u\n",
                   msg.source_id, msg.sample, msg.tick);

            if ((received % 5u) == 0u) {
                if (xSemaphoreTake(g_stats_mutex, portMAX_DELAY) == pdTRUE) {
                    printf("    -> stats: total=%u  last[0]=%u  last[1]=%u\n",
                           g_stats.total_samples,
                           g_stats.last_by_source[0],
                           g_stats.last_by_source[1]);
                    xSemaphoreGive(g_stats_mutex);
                }
            }
            /* Stop the demo after a while so the simulation exits cleanly. */
            if (received >= 20u) {
                printf("\n[logger] processed %u messages -- demo complete.\n",
                       received);
                vTaskEndScheduler();
            }
        }
    }
}

int main(void) {
    printf("FreeRTOS queue + mutex demo starting...\n");

    g_queue       = xQueueCreate(8, sizeof(sensor_msg_t));
    g_stats_mutex = xSemaphoreCreateMutex();
    if (g_queue == NULL || g_stats_mutex == NULL) {
        printf("failed to create RTOS objects\n");
        return 1;
    }

    /* Two producers at the same priority, one consumer slightly higher so it
     * drains the queue promptly. Priorities and stack depths are the two
     * knobs you always tune on real firmware. */
    xTaskCreate(sensor_task, "sensorA", configMINIMAL_STACK_SIZE,
                (void *)(uintptr_t)0, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(sensor_task, "sensorB", configMINIMAL_STACK_SIZE,
                (void *)(uintptr_t)1, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(logger_task, "logger",  configMINIMAL_STACK_SIZE * 2,
                NULL, tskIDLE_PRIORITY + 2, NULL);

    vTaskStartScheduler();      /* never returns while the scheduler runs */

    printf("scheduler stopped.\n");
    return 0;
}

/* ---- Hooks the kernel calls on error conditions ------------------------- */
void vApplicationMallocFailedHook(void) {
    printf("MALLOC FAILED -- increase configTOTAL_HEAP_SIZE\n");
    for (;;) { }
}

void vApplicationStackOverflowHook(TaskHandle_t task, char *name) {
    (void)task;
    printf("STACK OVERFLOW in task %s\n", name);
    for (;;) { }
}
