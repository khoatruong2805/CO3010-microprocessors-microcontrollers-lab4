/*
 * scheduler.c
 *
 *  Created on: Nov 13, 2026
 *      Author: ADMIN
 */

#include "scheduler.h"
#include <stddef.h>

#define SCH_MAX_TASKS 6U

typedef struct {
    void (*callback)(void);
    uint32_t start;
    uint32_t delay;
    uint32_t period;
} Task;

static volatile uint32_t tick_count = 0;
static Task tasks[SCH_MAX_TASKS];

void SCH_Update(void)
{
    tick_count++;
}

uint32_t SCH_Add_Task(void (*callback)(void), uint32_t delay, uint32_t period)
{
    if (callback == NULL) {
        return 0;
    }
    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++) {
        if (tasks[i].callback == NULL) {
            tasks[i] = (Task){callback, tick_count, delay, period};
            return i + 1;
        }
    }
    return 0;
}

uint8_t SCH_Delete_Task(uint32_t taskID)
{
    if (taskID == 0 || taskID > SCH_MAX_TASKS) {
        return 0;
    }

    Task *task = &tasks[taskID - 1];
    if (task->callback == NULL) {
        return 0;
    }

    *task = (Task){0};
    return 1;
}

void SCH_Dispatch_Tasks(void)
{
    uint32_t now = tick_count;
    uint32_t max_lateness = 0;
    Task *next = NULL;

    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++) {
        Task *task = &tasks[i];
        uint32_t elapsed = now - task->start;
        if (task->callback == NULL || elapsed < task->delay) {
            continue;
        }
        uint32_t lateness = elapsed - task->delay;
        if (next == NULL || lateness > max_lateness) {
            next = task;
            max_lateness = lateness;
        }
    }
    if (next == NULL) {
        return;
    }
    void (*callback)(void) = next->callback;
    if (next->period == 0) {
        next->callback = NULL;
    } else {
        next->start += next->delay;
        next->delay = next->period;
    }
    callback();
}

uint32_t SCH_Get_Time(void)
{
    return tick_count * 10U;
}
