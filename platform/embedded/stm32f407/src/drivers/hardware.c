#include <stdio.h>
#include <stdarg.h>
#include <lut.h>
#include "stm32f4xx_conf.h"
#include "ssd1289.h"
#include "nesgamepad.h"
#include "ucrtos.h"
#include "display.h"
#include "../../core/ucrtos.h"
#include "../../../eMIDI/src/midifile.h"
#include "../../../../../lib/LockFreeFifo/LockFreeFifo.h"

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
}

void hardwareDisplayDraw() {
  for(int y = 0; y < _yMax; ++y)
    for(int x = 0; x < _xMax; ++x) {
      int idx = y * _xMax + x;
      int subIdx = idx / 2;
      int isHigh = idx % 2 == 0;
      uint8_t ci = 0;

      if(!isHigh)
        ci = _pFrameBuffer[subIdx] & 0x0F;
      else
        ci = (_pFrameBuffer[subIdx] & 0xF0) >> 4;

      SSD1289_SetPoint(x, y, RGB565CONVERT(pMasterPalette[ci].red, pMasterPalette[ci].green, pMasterPalette[ci].blue));
    }
}

// MIDI device:

void hardwareInitMidiDevice() {
  // TODO: implement
}

void hardwareFreeMidiDevice() {
  // TODO: implement
}

void hardwareSendMidiMsg(const MidiEvent* pEvent) {
  // TODO: implement
}

// Input Device
void hardwareInputDeviceInit() {
  setupNesGamePad();
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

void hal_printf(char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);
  myvprintf(pFormat, args);
  va_end(args);
}

void hal_printfWarning(char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);
  myvprintf(pFormat, args);
  va_end(args);
}

void hal_printfError(char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);
  myvprintf(pFormat, args);
  va_end(args);
}

void hal_strcpy_s(char* dst, int maxSize, const char* src) {
  strcpy(dst, src);
}

// RS485:

static LockFreeFIFO_t* _pFifoDebugPort = 0;

void hal_rs485init(LockFreeFIFO_t* pFifo) {
  _pFifoDebugPort = pFifo;
}

void hal_rs485Send(char dataByte) {
  while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
  USART_SendData(USART6, dataByte);
}

void USART1_IRQHandler() {
  // check if the USART1 receive interrupt flag was set
  if(USART_GetITStatus(USART1, USART_IT_RXNE))
    writeToRingBuffer(_pFifoDebugPort, USART1->DR); // The Interrupt gets released just by reading USART1->DR
}
