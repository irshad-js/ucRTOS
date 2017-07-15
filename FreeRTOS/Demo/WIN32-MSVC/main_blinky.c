#include <stdio.h>
#include <conio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#define mainQUEUE_RECEIVE_TASK_PRIORITY	(tskIDLE_PRIORITY + 2)
#define	mainQUEUE_SEND_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)
#define mainTASK_SEND_FREQUENCY_MS			pdMS_TO_TICKS(200UL)
#define mainTIMER_SEND_FREQUENCY_MS			pdMS_TO_TICKS(2000UL)
#define mainQUEUE_LENGTH					      (10)

static void _prvQueueReceiveTask(void *pParameters);
static void _prvQueueSendTask(void *pParameters);
static void _prvQueueSendTimerCallback(TimerHandle_t xTimerHandle);

static QueueHandle_t _xQueue = NULL;
static TimerHandle_t _xTimer = NULL;
static SemaphoreHandle_t _xTaskMutex = NULL;

void main_blinky(void) {
  const TickType_t xTimerPeriod = mainTIMER_SEND_FREQUENCY_MS;
  _xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));

  if(_xQueue != NULL) {
    xTaskCreate(_prvQueueReceiveTask, "Rx", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);

    uint32_t vt1[] = {1, 2, 3};
    uint32_t vt2[] = {4, 5, 6};

    _xTaskMutex = xSemaphoreCreateMutex();
    xTaskCreate(_prvQueueSendTask, "Tx1", configMINIMAL_STACK_SIZE, vt1, mainQUEUE_SEND_TASK_PRIORITY, NULL);
    xTaskCreate(_prvQueueSendTask, "Tx2", configMINIMAL_STACK_SIZE, vt2, mainQUEUE_SEND_TASK_PRIORITY, NULL);

    _xTimer = xTimerCreate("Timer", xTimerPeriod, pdFALSE, NULL, _prvQueueSendTimerCallback);

    vTaskStartScheduler();
  }

  for(;;);
}

static void _prvQueueSendTask(void *pParameters) {
  uint32_t pValuesToSend[3];

  for (int i = 0; i < 3; ++i)
    pValuesToSend[i] = ((uint32_t*)pParameters)[i];

  TickType_t xNextWakeTime = xTaskGetTickCount();
  const TickType_t xBlockTime = mainTASK_SEND_FREQUENCY_MS;

  for(;;) {
    vTaskDelayUntil(&xNextWakeTime, xBlockTime);
    xSemaphoreTake(_xTaskMutex, portMAX_DELAY);

    int numElems = sizeof(pValuesToSend) / sizeof(uint32_t);

    for(int i = 0; i < numElems; ++i)
      xQueueSend(_xQueue, &pValuesToSend[i], 0U);

    xSemaphoreGive(_xTaskMutex);
  }
}

static void _prvQueueSendTimerCallback(TimerHandle_t xTimerHandle) {
  const uint32_t ulValueToSend = 3;
  (void) xTimerHandle;

  xQueueSend(_xQueue, &ulValueToSend, 0U);
}

static void _prvQueueReceiveTask(void *pParameters) {
  uint32_t ulReceivedValue;
  (void) pParameters;

  for(;;) {
    xQueueReceive(_xQueue, &ulReceivedValue, portMAX_DELAY);
    printf("Value: %d\n", ulReceivedValue);

    if (_kbhit() != 0) {
      (void)_getch();

      xTimerReset(_xTimer, portMAX_DELAY);
    }
  }
}
