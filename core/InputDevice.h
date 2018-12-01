#ifndef _INPUTDEVICE_H
#define _INPUTDEVICE_H

#include <stdint.h>
#include "StackBasedFsm.h"

typedef struct {
  uint16_t
    East      : 1,
    West      : 1,
    South     : 1,
    North     : 1,
    Start     : 1,
    Select    : 1,
    Back      : 1,
    Action    : 1,
    Connected : 1;
} InputDeviceStates_t;

void inputDeviceInit();
InputDeviceStates_t getInputDeviceState();
void debugPrintInputDeviceState();
void processInputDevice(StackBasedFsm_t* pFsm);
void hardwareInputDeviceInit();

#endif // _INPUTDEVICE_H

