#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "FreeRTOS.h"
#include "task.h"

#define mainREGION_1_SIZE	7001
#define mainREGION_2_SIZE	18105
#define mainREGION_3_SIZE	2807

extern void main_blinky(void);
static void prvInitialiseHeap( void );

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize);

StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

int main(void) {
  prvInitialiseHeap();
  main_blinky();

  return 0;
}

void vApplicationMallocFailedHook(void) {
  vAssertCalled( __LINE__, __FILE__ );
}

void vApplicationIdleHook(void) {

}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
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

