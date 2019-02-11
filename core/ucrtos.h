#ifndef UCRTOS_H__
#define UCRTOS_H__

#include <stdint.h>
#include "../lib/FreeRTOS/Source/include/FreeRTOS.h"
#include "../lib/FreeRTOS/Source/include/task.h"

#define pdUS_TO_TICKS(xTimeInUs) ((TickType_t)(((TickType_t)(xTimeInUs) * (TickType_t )configTICK_RATE_HZ) / (TickType_t)1000000))

typedef struct {
  char fileName[256];
} FO_FIND_DATA;

// Timing
uint32_t upTimeMs();
void delayUs(uint32_t us);
void delayMs(uint32_t ms);

// LEDs:
void statusLedOn();
void statusLedOff();
void errorLedOn();
void errorLedOff();

// FSM
#include "StackBasedFsm.h"
#include "SlotBasedMenu.h"

// Display:
#include "display.h"
#include "images.h"

// Input device:
#include "InputDevice.h"

// Debug:
void PrintCharUsr(char c);
void errorState();

#endif // UCRTOS_H__
