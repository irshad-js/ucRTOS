#include <stdlib.h>
#include <stdbool.h>
#include "mystdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
// #include "tm_stm32f4_fatfs.h" // TODO: implement proper abstraction layer
#include "InputDevice.h"
#include "screens/mainmenuscreen.h"
#include "ucrtos.h"

#define mainDEFAULT_TASK_PRIORITY	(tskIDLE_PRIORITY + 1)

static void _prvBlinkTask(void *pParameters) {
  (void*)pParameters;

  const int blinkFrequenyUs = 100000;

  while (1) {
    // myprintf("blink\n");
    statusLedOn();
    delayUs(blinkFrequenyUs);
    statusLedOff();
    delayUs(blinkFrequenyUs);
    statusLedOn();
    delayUs(blinkFrequenyUs);
    statusLedOff();
    delayUs(10 * blinkFrequenyUs);
  }
}

static void _prvDebugTask(void* pParameters) {
  (void*)pParameters;

  for(int i = 1000;; i++) {
    delayMs(250);
    myprintf("Tick: %i, upTimeMs: %u ms\n", i, upTimeMs());
  }
}

static void _prvDisplayTask(void* pParameters) {
  (void*)pParameters;

  myprintf("Display task started\n");
  displayInit();
  inputDeviceInit();
  
  StackBasedFsm_t fsm;
  fsmInit(&fsm);
  fsmPush(&fsm, mainMenuScreen, 0);

  while (1) {
    fsmTick(&fsm);
  }
}

static void _prvGamePadTask(void* pParams) {
  while(1) {
    InputDeviceStates_t state = getInputDeviceState();

    myprintf("Gamepad state:\n");
    myprintf("Action: %d\n", state.Action);
    myprintf("Back:   %d\n", state.Back);
    myprintf("North:  %d\n", state.North);
    myprintf("East:   %d\n", state.East);
    myprintf("South:  %d\n", state.South);
    myprintf("West:   %d\n", state.West);
    myprintf("Start:  %d\n", state.Start);
    myprintf("Select: %d\n", state.Select);
    myprintf("\n");

    delayMs(250);
  }
}

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

uint32_t upTimeMs() {
  return xTaskGetTickCount() / (configTICK_RATE_HZ / 1000);
}

void delayUs(uint32_t us) {
  vTaskDelay(pdUS_TO_TICKS(us));
}

void delayMs(uint32_t ms) {
  vTaskDelay(pdMS_TO_TICKS(ms));
}

int coreMain(void) {
  statusLedOff();
  // xTaskCreate(_prvDebugTask,   "Debug Task", 512, NULL, mainDEFAULT_TASK_PRIORITY, NULL);
  xTaskCreate(_prvBlinkTask,   "Blink Task", configMINIMAL_STACK_SIZE, NULL, mainDEFAULT_TASK_PRIORITY, NULL);
  xTaskCreate(_prvDisplayTask, "Display Task", 2048, NULL, mainDEFAULT_TASK_PRIORITY, NULL);
  // xTaskCreate(_prvGamePadTask, "Game pad Task", 512, NULL, mainDEFAULT_TASK_PRIORITY, NULL);
  // xTaskCreate(_prvFileSystemTask, "File System Task", 1024, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);

  vTaskStartScheduler();

  return 0;
}
