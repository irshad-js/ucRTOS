#include "NesGamePad.h"
#include "stm32f4xx_conf.h"

// TODO: implement HAL

void setupNesGamePad() {
  // SPI pins for game pad:

  // PB2: latch
  // PB10: Clock
  // PC2: Data (MISO)
  GPIO_InitTypeDef GPIO_InitStruct;
  SPI_InitTypeDef SPI_InitStruct;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2; // MISO (MOSI is not needed and therefore not connected)
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource2,  GPIO_AF_SPI2);

  // configure SPI2 in Mode 0
  // CPOL = 1 --> clock is high when idle
  // CPHA = 0 --> data is sampled at the first edge

  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   // set to full duplex mode, seperate MOSI and MISO lines (should be SPI_Direction_1Line_Rx)
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;                        // transmit in master mode, NSS pin has to be always high
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;                    // one packet of data is 8 bits wide
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;                          // clock is high when idle
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;                         // data sampled at first edge
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;  // set the NSS management to internal and pull internal NSS high
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128; // SPI frequency is APB1 frequency / 128
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;                   // data is transmitted MSB first
  SPI_Init(SPI2, &SPI_InitStruct);

  SPI_Cmd(SPI2, ENABLE);
}

static void prvLatchShiftRegister() {
  GPIOB->BSRRL |= GPIO_Pin_2; // Latch high
  delayUs(12);
  GPIOB->BSRRH |= GPIO_Pin_2; // Latch low
  delayUs(12);
}

union NesGamePadStates_t getNesGamepadState() {
  union NesGamePadStates_t states;
  prvLatchShiftRegister();

  SPI2->DR = 0x00;                        // It doesen't matter which byte is send here, since MOSI is not connected.
  while(!(SPI2->SR & SPI_I2S_FLAG_TXE));  // wait until transmit complete
  while(!(SPI2->SR & SPI_I2S_FLAG_RXNE)); // wait until receive complete
  while(SPI2->SR & SPI_I2S_FLAG_BSY);     // wait until SPI is not busy anymore
  states.code = SPI2->DR;
  states.code ^= 0xFF;

  return states;
}

