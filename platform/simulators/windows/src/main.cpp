#include <wx/wx.h>

extern "C" {
  #include "FreeRTOS.h"
  #include "task.h"
  #include <stdio.h>
}

StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

extern "C" {
  int coreMain(void);

  void vApplicationMallocFailedHook(void) {
    vAssertCalled(__LINE__, __FILE__);
  }

  void vApplicationIdleHook(void) {

  }

  void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
    (void)pcTaskName;
    (void)pxTask;

    vAssertCalled(__LINE__, __FILE__);
  }

  void vApplicationTickHook(void) {

  }

  void vApplicationDaemonTaskStartupHook(void) {

  }

  void vAssertCalled(unsigned long ulLine, const char * const pcFileName) {
    static portBASE_TYPE xPrinted = pdFALSE;
    volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

    (void)ulLine;
    (void)pcFileName;

    printf("ASSERT! Line %d, file %s, GetLastError() %d\r\n", ulLine, pcFileName, GetLastError());

    while (ulSetToNonZeroInDebuggerToContinue == 0) {
      __asm { NOP };
      __asm { NOP };
    }
  }

  void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
    uint32_t *pulIdleTaskStackSize) {
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

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
}

int main(void) {
  return coreMain();
}