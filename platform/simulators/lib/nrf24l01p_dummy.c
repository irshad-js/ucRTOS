#include <stdint.h>
#include "nrf24l01p.h"
#include "nrf24_hal.h"

uint8_t nrf24_shockburstIsEnabled(uint8_t pipeId) {
  return 0;
}

// NRF24 API
void nrf24_enableCRC(uint8_t numBytes) {

}

void nrf24_powerUp(uint8_t enable) {

}

void nrf24_listenMode(uint8_t enable) {

}

// EN_AA
void nrf24_enableShockburst(uint8_t pipeId, uint8_t enable) {

}

// EN_RXADDR
void nrf24_enableDataPipe(uint8_t pipeId, uint8_t enable) {

}

// SETUP_AW
void nrf24_setAddressWidth(uint8_t numBytes) {

}

// RF_CH
void nrf24_setRFChannel(uint8_t channel) {

}

// RF_SETUP
void nrf24_setDataRate(uint8_t dataRate) {

}

void nrf24_setXmitPower(uint8_t powerLevel) {

}

// TODO: refactor for subaddresses?
void nrf24_setRxAddress(uint8_t pipeId, uint8_t* pRxAddress) {

}

void nrf24_setTxAddress(uint8_t* pAddress) {

}

void nrf24_setPayloadSize(uint8_t pipeId, uint8_t size) {

}

uint8_t nrf24_getRxAddress(uint8_t pipeId, uint8_t* pRxAddress) {

}

uint8_t nrf24_getTxAddress(uint8_t* pTxAddress) {

}

uint8_t nrf24_crcIsEnabled() {
  return 0;
}

uint8_t nrf24_getRFChannel() {
  return 0;
}

uint8_t nrf24_crcGetEncodingScheme() {
  return 0;
}

uint8_t nrf24_isPoweredOn() {
  return 1;
}

uint8_t nrf24_dataPipeIsEnabled(uint8_t pipeId) {
  return 0;
}

uint8_t nrf24_getAddressWidths() {
  return 0;
}

uint8_t nrf24_getCurrentRxPipe() {
  return 0;
}

uint8_t nrf24_getDataRate() {
  return SPEED_1M;
}

uint8_t nrf24_isListening() {
  return 0;
}

uint8_t nrf24_getPayloadSize(uint8_t pipeId) {
  return 0;
}

uint8_t nrf24_getPayloadSizeRxFifoTop() {
  return 0;
}

uint32_t nrf24_recvPacket(void* pPacket) {
  return 0;
}

void nrf24_flushRxFifo() {

}

void nrf24_flushTxFifo() {

}

int8_t nrf24_sendPacket(void* pPacket, int8_t payloadSize, uint8_t listenAfterSend) {
  hal_printf("nrf24_sendPacket [%d]: ");

  for (int i = 0; i < payloadSize; ++i) {
    uint8_t* p = (uint8_t*)pPacket;

    hal_printf("%02X ", p[i]);
  }

  hal_printf("\n");

  return NRF_OK;
}

uint8_t nrf24_txFifoIsFull() {
  return 0;
}

uint8_t nrf24_txFifoIsEmpty() {
  return 0;
}

uint8_t nrf24_carrierIsPresent() {
  return 0;
}

void nrf24_init() {

}

