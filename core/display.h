#pragma once

#include <stdint.h>

void displayInit();
void displaySetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue);
void displayDraw();
void displayClear();
void displayDrawImage(uint16_t xPos, uint16_t yPos, const uint8_t* pImg);

// Platform driver implementations:
void hardwareDisplayInit();
void hardwareDisplaySetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue);
void hardwareDisplayDraw();
void hardwareDisplayClear();
