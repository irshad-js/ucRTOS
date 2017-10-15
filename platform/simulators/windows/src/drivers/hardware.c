#include <windows.h>
#include <stdio.h>
#include "../../core/ucrtos.h"

void statusLedOn() {
  if (GetKeyState(VK_SCROLL) == 1)
    return;

  keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
  keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

void statusLedOff() {
  if (GetKeyState(VK_SCROLL) == 0)
    return;

  keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
  keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

void PrintCharUsr(char c) {
  putchar(c);
}

// Display:
void mainCreateWxLcdSimulator();
void mainLcdSetPixel(int x, int y, int red, int green, int blue);
void mainLcdDraw();
void mainLcdClear();

void displayInit() {
  mainCreateWxLcdSimulator();
}

void displaySetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) {
  mainLcdSetPixel(x, y, red, green, blue);
}

void displayDraw() {
  mainLcdDraw();
}

void displayClear() {
  mainLcdClear();
}
