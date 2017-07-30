#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "stm32f4xx_conf.h"
#include "ucrtos.h"
#include "main.h"

int main(void) {
  init();

  return coreMain();
}

void init() {
  GPIO_InitTypeDef GPIO_InitStructure;

  // ---------- SysTick timer -------- //
  if (SysTick_Config(SystemCoreClock / 1000)) {
    // Capture error
	while (1) {};
  }

  // Init LEDs. The red and blue LEDs (Pin 14 / 15) are used by FSMC and cannot be used:
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

// Dummy function to avoid compiler error
void _init() {

}
