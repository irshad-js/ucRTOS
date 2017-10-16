#include <stdlib.h>
#include <stdbool.h>
#include "mystdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
// #include "tm_stm32f4_fatfs.h" // TODO: implement proper abstraction layer
// #include "nesgamepad.h"       // TODO: implement proper abstraction layer
#include "states/mainMenu.h"
#include "ucrtos.h"

#define mainREGION_1_SIZE	7001
#define mainREGION_2_SIZE	18105
#define mainREGION_3_SIZE	2807

#define mainDEFAULT_TASK_PRIORITY	(tskIDLE_PRIORITY + 1)
#define blinkTASK_FREQUENY_MS     			pdMS_TO_TICKS(100UL)

static void prvInitialiseHeap( void );

static void _prvBlinkTask(void *pParameters) {
  (void*)pParameters;

  while (1) {
    myprintf("blink\n");
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
  (void*)pParameters;

  for(int i = 1000;; i++) {
    vTaskDelay(pdMS_TO_TICKS(250UL));
    myprintf("Tick: %i\n", i);
  }
}

static void _prvDisplayTask(void* pParameters) {
  (void*)pParameters;

  myprintf("Display task started\n");

  displayInit();

  // for (int x = 0; x < 320; ++x) {
  //   for (int y = 0; y < 240; ++y)
  //     displaySetPixel(x, y, 0, 0, 0);
  // }
  //
  // displayDrawImage(100, 100, _pCursorImg);
  // displayDrawText(100, 200, "Test String", 255, 255, 255, 0, 0, 0);
  //
  // displayDraw();


  StackBasedFsm_t fsm;

  fsmInit(&fsm);
  fsmPush(&fsm, mainMenu, 0);

  while (1) {
    fsmTick(&fsm);
  }
}

// TODO: implement proper HAL for game pad:

// static void _prvGamePadTask(void* pParams) {
//   setupNesGamePad();
//   union NesGamePadStates_t state;
//
//   while(1) {
//     state = getNesGamepadState();
//
//     myprintf("Gamepad state:\n");
//     myprintf("A:      %d\n", state.states.A);
//     myprintf("B:      %d\n", state.states.B);
//     myprintf("North:  %d\n", state.states.North);
//     myprintf("East:   %d\n", state.states.East);
//     myprintf("South:  %d\n", state.states.South);
//     myprintf("West:   %d\n", state.states.West);
//     myprintf("Start:  %d\n", state.states.Start);
//     myprintf("Select: %d\n", state.states.Select);
//     myprintf("\n", state.states.Select);
//
//     vTaskDelay(pdMS_TO_TICKS(250UL));
//   }
// }

// -

// TODO: implement proper HAL for file system:

// static bool hal_findNext(DIR* pDir, FILINFO* pFinfo) {
//   FRESULT error;
//
//   if (error = f_readdir(pDir, pFinfo))
//     return false;
//
//   bool foundFile = pFinfo->fname[0];
//
//   if(!foundFile)
//     return false;
//
//   return true;
// }
//
// static bool hal_findInit(DIR* pDir, const char* pPath, FILINFO* pFinfo) {
//   FRESULT error;
//
//   if(error = f_opendir(pDir, pPath))
//     return false;
//
//   bool foundFile = hal_findNext(pDir, pFinfo);
//
//   if(!foundFile)
//     return false;
//
//   return true;
// }
//
// void hal_findFree(DIR* pDir) {
//   return  f_closedir(pDir) != FR_OK;
// }
//
// static void _prvFileSystemTask(void* pParameters) {
//   DIR dir;
//   FILINFO finfo;
//   FATFS FatFs;
//   char pLfn[_MAX_LFN] = {0};
//   finfo.lfname = pLfn;
//   finfo.lfsize = _MAX_LFN;
//
//   uint32_t total = 0;
//   uint32_t free = 0;
//
//   while(1) {
//     f_mount(&FatFs, "0:", 1);
//
//     if (TM_FATFS_DriveSize(&total, &free) == FR_OK) {
//       myprintf("Total: %d Bytes\n", total);
//       myprintf("Free: %d Bytes\n", free);
//     }
//
//     bool endOfDirectory = !hal_findInit(&dir, "/", &finfo);
//     int curFileIndex = 0;
//
//     while (!endOfDirectory) {
//       const char* p = finfo.lfname[0] ? finfo.lfname : finfo.fname;
//
//       if (p[0] != '.' && p[1] != '.') {
//         myprintf("%s\n", p);
//         endOfDirectory = !hal_findNext(&dir, &finfo);
//       }
//     }
//
//     hal_findFree(&dir);
//     f_mount(0, "", 1); // unmount
//   }
//
//   while(1);
// }

int coreMain(void) {
  prvInitialiseHeap();
  statusLedOff();
  xTaskCreate(_prvDebugTask,   "Debug Task", 512, NULL, mainDEFAULT_TASK_PRIORITY, NULL);
  xTaskCreate(_prvBlinkTask,   "Blink Task", configMINIMAL_STACK_SIZE, NULL, mainDEFAULT_TASK_PRIORITY, NULL);
  xTaskCreate(_prvDisplayTask, "Display Task", 1024, NULL, mainDEFAULT_TASK_PRIORITY, NULL);

  // xTaskCreate(_prvFileSystemTask, "File System Task", 1024, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);
  // xTaskCreate(_prvGamePadTask,      "Game pad Task", 512, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);

  vTaskStartScheduler();

  return 0;
}

static void prvInitialiseHeap(void) {
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
