#pragma once

#include <stdint.h>
#include <limits.h>
#define CENTER USHRT_MAX

typedef struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} Color;

extern Color pPalette[16];

void displayInit();
void displaySetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue);
void displayDraw();
void displayClear();
void displayDrawImage(uint16_t xPos, uint16_t yPos, const uint8_t* pImg);
void displayDrawText(uint16_t x, uint16_t y, const char* str, uint8_t txtRed, uint8_t txtGreen, uint8_t txtBlue,
    uint8_t bgRed, uint8_t bgGreen, uint8_t bgBlue);

// Platform driver implementations:
void hardwareDisplayInit(uint8_t* hardwareDisplayInit, uint8_t* pFrameBufferOld, int xMax, int yMax);
void hardwareDisplayDraw();
