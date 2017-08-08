#include <stdlib.h>
#include <stdbool.h>
#include "mystdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "tm_stm32f4_fatfs.h"
#include "nesgamepad.h"
#include "ucrtos.h"

#define mainREGION_1_SIZE	7001
#define mainREGION_2_SIZE	18105
#define mainREGION_3_SIZE	2807

#define mainQUEUE_RECEIVE_TASK_PRIORITY	(tskIDLE_PRIORITY + 2)
#define	mainQUEUE_SEND_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)
#define mainTASK_SEND_FREQUENCY_MS			pdMS_TO_TICKS(200UL)
#define mainTIMER_SEND_FREQUENCY_MS			pdMS_TO_TICKS(2000UL)
#define mainQUEUE_LENGTH					      (10)
#define blinkTASK_FREQUENY_MS     			pdMS_TO_TICKS(100UL)

static void _prvQueueReceiveTask(void *pParameters);
static void _prvQueueSendTask(void *pParameters);
static void _prvQueueSendTimerCallback(TimerHandle_t xTimerHandle);
static void prvInitialiseHeap( void );

static QueueHandle_t _xQueue = NULL;
static TimerHandle_t _xTimer = NULL;
static SemaphoreHandle_t _xTaskMutex = NULL;

static void _prvBlinkTask(void *pParameters) {
  while (1) {
    statusLedOn();
    vTaskDelay(blinkTASK_FREQUENY_MS);
    statusLedOff();
    vTaskDelay(blinkTASK_FREQUENY_MS);
    statusLedOn();
    vTaskDelay(blinkTASK_FREQUENY_MS);
    statusLedOff();
    vTaskDelay(10 * blinkTASK_FREQUENY_MS);
  }
}

static void _prvDebugTask(void* pParameters) {
  for(int i = 1000;; i++) {
    vTaskDelay(pdMS_TO_TICKS(10UL));
    // myprintf("Tick: %i\n", i);
  }
}

static void _prvGamePadTask(void* pParams) {
  setupNesGamePad();
  union NesGamePadStates_t state;

  while(1) {
    state = getNesGamepadState();

    myprintf("Gamepad state:\n");
    myprintf("A:      %d\n", state.states.A);
    myprintf("B:      %d\n", state.states.B);
    myprintf("North:  %d\n", state.states.North);
    myprintf("East:   %d\n", state.states.East);
    myprintf("South:  %d\n", state.states.South);
    myprintf("West:   %d\n", state.states.West);
    myprintf("Start:  %d\n", state.states.Start);
    myprintf("Select: %d\n", state.states.Select);
    myprintf("\n", state.states.Select);

    vTaskDelay(pdMS_TO_TICKS(250UL));
  }
}

static bool hal_findNext(DIR* pDir, FILINFO* pFinfo) {
  FRESULT error;

  if (error = f_readdir(pDir, pFinfo))
    return false;

  bool foundFile = pFinfo->fname[0];

  if(!foundFile)
    return false;

  return true;
}

static bool hal_findInit(DIR* pDir, const char* pPath, FILINFO* pFinfo) {
  FRESULT error;

  if(error = f_opendir(pDir, pPath))
    return false;

  bool foundFile = hal_findNext(pDir, pFinfo);

  if(!foundFile)
    return false;

  return true;
}

void hal_findFree(DIR* pDir) {
  return  f_closedir(pDir) != FR_OK;
}

static void _prvFileSystemTask(void* pParameters) {
  DIR dir;
  FILINFO finfo;
  FATFS FatFs;
  char pLfn[_MAX_LFN] = {0};
  finfo.lfname = pLfn;
  finfo.lfsize = _MAX_LFN;

  uint32_t total = 0;
  uint32_t free = 0;

  while(1) {
    f_mount(&FatFs, "0:", 1);

    if (TM_FATFS_DriveSize(&total, &free) == FR_OK) {
      myprintf("Total: %d Bytes\n", total);
      myprintf("Free: %d Bytes\n", free);
    }

    bool endOfDirectory = !hal_findInit(&dir, "/", &finfo);
    int curFileIndex = 0;

    while (!endOfDirectory) {
      const char* p = finfo.lfname[0] ? finfo.lfname : finfo.fname;

      if (p[0] != '.' && p[1] != '.') {
        myprintf("%s\n", p);
        endOfDirectory = !hal_findNext(&dir, &finfo);
      }
    }

    hal_findFree(&dir);
    f_mount(0, "", 1); // unmount
  }

  while(1);
}

int coreMain(void) {
  prvInitialiseHeap();
  statusLedOff();
  xTaskCreate(_prvBlinkTask,      "Blink Task", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);
  xTaskCreate(_prvDebugTask,      "Debug Task", 512, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);
  // xTaskCreate(_prvFileSystemTask, "File System Task", 1024, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);
  xTaskCreate(_prvGamePadTask,      "Game pad Task", 512, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);

  vTaskStartScheduler();

  return 0;



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
    myprintf("Value: %lu\n", ulReceivedValue);
  }
}

static void  prvInitialiseHeap(void) {
  static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
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
