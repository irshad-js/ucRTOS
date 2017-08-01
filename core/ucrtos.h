#ifndef UCRTOS_H__
#define UCRTOS_H__

#include <stdint.h>

// Timing
void delayUs(uint32_t us);
void delayMs(uint32_t ms);

// LEDs:
void statusLedOn();
void statusLedOff();
void errorLedOn();
void errorLedOff();

// Display:
void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);

// Debug:
void errorState();

#endif // UCRTOS_H__
