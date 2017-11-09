#include <stdio.h>
#include "stm32f4xx_conf.h"
#include "ssd1289.h"
#include "nesgamepad.h"
#include "ucrtos.h"

// Timing

void delayUs(uint32_t us) {
  TIM2->CNT = 0;
  while(TIM2->CNT < us);
}

void delayMs(uint32_t ms) {
  for(int i = 0; i < ms; i++)
    delayUs(1000);
}

// LED

void statusLedOn() {
  GPIO_SetBits(GPIOD, GPIO_Pin_12);
}

void statusLedOff() {
  GPIO_ResetBits(GPIOD, GPIO_Pin_12);
}

void errorLedOn() {
  GPIO_SetBits(GPIOD, GPIO_Pin_13);
}

void errorLedOff() {
  GPIO_ResetBits(GPIOD, GPIO_Pin_13);
}

void errorState() {
  while(1) {
    errorLedOn();
    delayMs(50);
    errorLedOff();
    delayMs(500);
  }
}

// Display

static uint8_t* _pFrameBuffer = 0;
static int _xMax = 0;
static int _yMax = 0;

void hardwareDisplayInit(uint8_t* pFrameBuffer, int xMax, int yMax) {
  _pFrameBuffer = pFrameBuffer;
  _xMax = xMax;
  _yMax = yMax;

  SSD1289_Init();
  SSD1289_Clear(Black);

  setupNesGamePad(); // TODO: create hardwareInputInit function
}

void hardwareDisplayDraw() {
  for(int y = 0; y < _yMax; ++y)
    for(int x = 0; x < _xMax; ++x) {
      typedef struct Bla {
        uint8_t
        red   : 2,
        green : 3,
        blue  : 2;
      };

      int index = y * _xMax + x;
      struct Bla* p = (struct Bla*)&_pFrameBuffer[index];

      uint8_t red   = (p->red   * 255) / 3;
      uint8_t green = (p->green * 255) / 7;
      uint8_t blue  = (p->blue  * 255) / 3;

      SSD1289_SetPoint(x, y, RGB565CONVERT(red, green, blue));
    }
}

// Dummy function to avoid compiler error (Is called by libc_init_array()):

void _init() {
  // Just leave empty
}

// Debug Interface:

void PrintCharUsr(char c) {
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); // Wait until transmit finishes
  USART_SendData(USART1, (u8) c);
}

uint32_t hal_clock() {
  return TIM5->CNT / 1000; // TODO: replace by FreeRTOS functions
}

InputDeviceStates_t getInputDeviceState() {
  InputDeviceStates_t states;
  union NesGamePadStates_t nesStates = getNesGamepadState();

  states.Action = nesStates.states.A;
  states.Back   = nesStates.states.B;
  states.East   = nesStates.states.East;
  states.West   = nesStates.states.West;
  states.North  = nesStates.states.North;
  states.South  = nesStates.states.South;
  states.Start  = nesStates.states.Start;
  states.Select = nesStates.states.Select;

  states.Connected = nesStates.code != 0xFF;

  return states;
}

void hal_printf(char* format, ...) {
  // TODO: implement
}

void hal_printfWarning(char* format, ...) {
  // TODO: implement
}

void hal_printfError(char* format, ...) {
  // TODO: implement
}

void hal_strcpy_s(char* dst, int maxSize, const char* src) {
  strcpy(dst, src);
}

