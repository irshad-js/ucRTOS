#include <stdio.h>
#include "stm32f4xx_conf.h"
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

void hardwareDisplayInit() {
  // TODO: implement
}

void hardwareDisplaySetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {

  // TODO: implement
}

void hardwareDisplayDraw() {
  // TODO: implement
}

void hardwareDisplayClear() {
  // TODO: implement
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
  return 0; // TODO: implement
}

InputDeviceStates_t getInputDeviceState() {
  InputDeviceStates_t states;

  // TODO: implement

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
  // TODO: implement
}


