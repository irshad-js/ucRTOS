#ifndef UCRTOS_H__
#define UCRTOS_H__

#include <stdint.h>


// Hardware HAL:

void onHwInit();
void onHwExit();
void statusLedOn();
void statusLedOff();
void blockMs(int32_t timeMs);

#endif // UCRTOS_H__
