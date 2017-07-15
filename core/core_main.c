#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#define mainREGION_1_SIZE	7001
#define mainREGION_2_SIZE	18105
#define mainREGION_3_SIZE	2807

#define mainQUEUE_RECEIVE_TASK_PRIORITY	(tskIDLE_PRIORITY + 2)
#define	mainQUEUE_SEND_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)
#define mainTASK_SEND_FREQUENCY_MS			pdMS_TO_TICKS(200UL)
#define mainTIMER_SEND_FREQUENCY_MS			pdMS_TO_TICKS(2000UL)
#define mainQUEUE_LENGTH					      (10)

static void _prvQueueReceiveTask(void *pParameters);
static void _prvQueueSendTask(void *pParameters);
static void _prvQueueSendTimerCallback(TimerHandle_t xTimerHandle);
static void prvInitialiseHeap( void );

static QueueHandle_t _xQueue = NULL;
static TimerHandle_t _xTimer = NULL;
static SemaphoreHandle_t _xTaskMutex = NULL;

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize);

StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

int coreMain(void) {
  prvInitialiseHeap();

  const TickType_t xTimerPeriod = mainTIMER_SEND_FREQUENCY_MS;
  _xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));

  if (_xQueue != NULL) {
    xTaskCreate(_prvQueueReceiveTask, "Rx", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);

    uint32_t vt1[] = {1, 2, 3};
    uint32_t vt2[] = {4, 5, 6};

    _xTaskMutex = xSemaphoreCreateMutex();
    xTaskCreate(_prvQueueSendTask, "Tx1", configMINIMAL_STACK_SIZE, vt1, mainQUEUE_SEND_TASK_PRIORITY, NULL);
    xTaskCreate(_prvQueueSendTask, "Tx2", configMINIMAL_STACK_SIZE, vt2, mainQUEUE_SEND_TASK_PRIORITY, NULL);

    _xTimer = xTimerCreate("Timer", xTimerPeriod, pdFALSE, NULL, _prvQueueSendTimerCallback);

    vTaskStartScheduler();
  }

  return 0;
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

void vApplicationMallocFailedHook(void) {
  vAssertCalled(__LINE__, __FILE__);
}

void vApplicationIdleHook(void) {

}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
  (void) pcTaskName;
  (void) pxTask;

  vAssertCalled(__LINE__, __FILE__);
}

void vApplicationTickHook(void) {

}

void vApplicationDaemonTaskStartupHook(void) {

}

void vAssertCalled(unsigned long ulLine, const char * const pcFileName) {
  static portBASE_TYPE xPrinted = pdFALSE;
  volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

  (void) ulLine;
  (void) pcFileName;

  printf("ASSERT! Line %d, file %s, GetLastError() %d\r\n", ulLine, pcFileName, GetLastError());

  while(ulSetToNonZeroInDebuggerToContinue == 0) {
    __asm{ NOP };
    __asm{ NOP };
  }
}

static void  prvInitialiseHeap(void) {
  static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
  volatile uint32_t ulAdditionalOffset = 19;

  const HeapRegion_t xHeapRegions[] = {
  { ucHeap + 1,   mainREGION_1_SIZE },
  { ucHeap + 15 + mainREGION_1_SIZE, mainREGION_2_SIZE },
  { ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE, mainREGION_3_SIZE },
  { NULL, 0 }
};

  configASSERT((ulAdditionalOffset + mainREGION_1_SIZE + mainREGION_2_SIZE + mainREGION_3_SIZE ) < configTOTAL_HEAP_SIZE);

  (void) ulAdditionalOffset;
  vPortDefineHeapRegions(xHeapRegions);
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
    uint32_t *pulIdleTaskStackSize) {
  static StaticTask_t xIdleTaskTCB;
  static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer,
    uint32_t *pulTimerTaskStackSize) {
  static StaticTask_t xTimerTaskTCB;

  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxTimerTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
