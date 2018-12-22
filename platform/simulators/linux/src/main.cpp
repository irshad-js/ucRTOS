#include <stdio.h>
#include <time.h>

extern "C" {
  int coreMain(void);
}

int main(int argc, char* pArgv[]) {
  printf("ucRTOS linux simulator\n");

  return coreMain();
}

//-----------------------------------------------------------------------------
// FreeRTOS Application Hooks
//-----------------------------------------------------------------------------

extern "C" void vApplicationIdleHook(void) {
  // The co-routines are executed in the idle task using the idle task hook.
  // vCoRoutineSchedule(); // Comment this out if not using Co-routines.

  // Makes the process more agreeable when using the Posix simulator:
  struct timespec xTimeToSleep, xTimeSlept;
  xTimeToSleep.tv_sec = 1;
  xTimeToSleep.tv_nsec = 0;
  nanosleep(&xTimeToSleep, &xTimeSlept);
}

extern "C" void vMainQueueSendPassed(void) {

}

