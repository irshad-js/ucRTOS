#include <stdio.h>
#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"
#include "nrf24l01p.h"
#include "stm32f4xx_conf.h"
#include "skateboardscreen.h"

static struct {
  int someLocalVariable;
  int* pReturnValue;

} context;

static void draw() {
  displayClear();

  displayDrawText(CENTER, 0 + 0 * 18, "Skateboard screen", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(30, 0 + 2 * 18, "Press 'A' button to accelerate", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(30, 0 + 3 * 18, "Press 'B' button to brake", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(30, 0 + 4 * 18, "Press 'Select' button to exit", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDraw();
}

static void onActionPress(StackBasedFsm_t* pFsm) {
  hal_printf("skateboard::onActionPress()");
}

static void onBackPress(StackBasedFsm_t* pFsm) {
  hal_printf("skateboard::onBackPress()");
}

static void onStartPress(StackBasedFsm_t* pFsm) {
  hal_printf("skateboard::onStartPress()");
}

static void onSelectPress(StackBasedFsm_t* pFsm) {
  hal_printf("skateboard::onSelectPress()");

  leaveState(pFsm);
}

static void onDirectionPress(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("skateboard::onDirectionPress()");
}

static void onActionRelease(StackBasedFsm_t* pFsm) {
  hal_printf("skateboard::onActionRelease()");
}

static void onBackRelease(StackBasedFsm_t* pFsm) {
  hal_printf("skateboard::onBackRelease()");
}

static void onStartRelease(StackBasedFsm_t* pFsm) {
  hal_printf("skateboard::onStartRelease()");
}

static void onSelectRelease(StackBasedFsm_t* pFsm) {
  hal_printf("skateboard::onSelectRelease()");
}

static void onReenter(StackBasedFsm_t* pFsm) {
  hal_printf("skateboard::onReenter()");
}

static void onLeaveState(StackBasedFsm_t* pFsm) {
  hal_printf("skateboard::onLeaveState()");

  if (!context.pReturnValue) {
    hal_printfWarning("Cannot return value. pReturnValue is null");
    return;
  }

  *context.pReturnValue = 42;
}

// -----------------------------------------

// Private variables
volatile uint32_t time_var1;

// Called from systick handler
void timing_handler() {
  if (time_var1)
    time_var1--;
}

// Delay a number of systick cycles (1ms)
static void Delay(volatile uint32_t nCount) {
  time_var1 = nCount;

  while(time_var1);
}

static void blink() {
  GPIO_SetBits(GPIOD, GPIO_Pin_12);
  Delay(50);
  GPIO_ResetBits(GPIOD, GPIO_Pin_12);
  Delay(50);
}

int sendUartChar(char ch) {
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
  USART_SendData(USART1, ch);

  return ch;
}

void sendUartText(const char* pText) {
  for(int i = 0; pText[i]; i++)
    sendUartChar(pText[i]);
}

void debugPrint(const char* pText) {
  sendUartText(pText);
}

void debugPrintln(const char* pText) {
  sendUartText(pText);
  sendUartChar('\n');
}

void debugPrintDec(int n) {
  char pNum[32];
  snprintf(pNum, sizeof(pNum), "%d", n);

  sendUartText(pNum);
}

void debugPrintHex(int n) {
  char pHex[32];
  snprintf(pHex, sizeof(pHex), "0x%02X", n);

  sendUartText(pHex);
}

void printFullConfig() {
  debugPrintln("NRF24 Configuration:");
  debugPrint("Mode: "); debugPrintln(nrf24_isListening() ? "Listening" : "Transmitting");
  debugPrint("RF Channel: "); debugPrintDec(nrf24_getRFChannel()); debugPrintln("");
  debugPrint("RF Speed: "); debugPrintln(nrf24_getDataRate() == SPEED_2M ? "2M" : nrf24_getDataRate() == SPEED_1M ? "1M" : "250K");
  debugPrint("CRC: "); debugPrintln(nrf24_crcIsEnabled() ? "Enabled" : "Disabled");
  debugPrint("Encoding scheme: "); debugPrintDec(nrf24_crcGetEncodingScheme()); debugPrintln("");
  debugPrint("Power Status: "); debugPrintln(nrf24_isPoweredOn() ? "On" : "Off");
  debugPrintln("Shockburst:");

  for(int i = 0; i < 6; i++) {
    debugPrint("  ENAA_P"); debugPrintDec(i);
    debugPrint(": ");
    debugPrintln(nrf24_shockburstIsEnabled(i) ? "Enabled" : "Disabled");
  }

  debugPrintln("RX Data Pipes:");
  for(int i = 0; i < 6; i++) {
    debugPrint("  ERX_P"); debugPrintDec(i); debugPrint(": ");
    debugPrintln(nrf24_dataPipeIsEnabled(i) ? "Enabled" : "Disabled");
  }

  debugPrintln("RX Payload sizes: ");
  for(int i = 0; i < 6; i++) {
    debugPrint("  RX_PW_P"); debugPrintDec(i); debugPrint(": ");
    debugPrintDec(nrf24_getPayloadSize(i)); debugPrintln("");
  }

  debugPrint("Address Width: "); debugPrintDec(nrf24_getAddressWidths()); debugPrintln("");
  debugPrintln("RX Addresses: ");

  uint8_t rxAddr[5];
  for(int pipeId = 0; pipeId < 6; pipeId++) {
    nrf24_getRxAddress(pipeId, rxAddr);
    debugPrint("  RX_ADDR_P"); debugPrintDec(pipeId); debugPrint(": ");

    for(int i = 0; i < nrf24_getAddressWidths(); i++) {
      debugPrintHex(rxAddr[i]);
      debugPrint(" ");
    }

    debugPrintln("");
  }

  debugPrintln("TX Address: ");
  uint8_t txAddr[5];
  nrf24_getTxAddress(txAddr);
  debugPrint("  TX_ADDR: ");

  for(int i = 0; i < nrf24_getAddressWidths(); i++) {
    debugPrintHex(txAddr[i]);
    debugPrint(" ");
  }

  debugPrintln("");

  uint8_t fifoIsFull = nrf24_txFifoIsFull();
  debugPrint("TX_FIFO: "); debugPrintln(fifoIsFull ? "Full" : "Free");
}

void setup_nrf24() {
  nrf24_init();

  uint8_t rxaddr[] = {0x01, 0x02, 0x03, 0x02, 0x01 };
  uint8_t txaddr[] = {0x01, 0x02, 0x03, 0x02, 0x01 };

  nrf24_setRxAddress(PIPE_0, rxaddr);
  nrf24_setTxAddress(txaddr);
  nrf24_enableCRC(CRC_MODE_OFF);
  nrf24_enableDataPipe(PIPE_0, TRUE);
  nrf24_enableDataPipe(PIPE_1, FALSE);
  nrf24_enableShockburst(PIPE_0, FALSE);
  nrf24_setAddressWidth(5);
  nrf24_listenMode(TRUE);
  nrf24_setDataRate(SPEED_2M);
  nrf24_setPayloadSize(PIPE_0, 16);
  nrf24_setRFChannel(81);
  nrf24_powerUp(TRUE);

  debugPrintln("NRF24L01+ Library Example");
  debugPrintln("-------------------------");
  printFullConfig();
}

static void init() {
  nrf24_init();
  setup_nrf24();
}

// -----------------------------------------

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("skateboard::onEnter()");

  SkateboardScreenParams* pSkateboardScreenParams = (SkateboardScreenParams*)pParams;
  if (!pParams)
    hal_printfWarning("Param is nullptr");
  else {
    context.someLocalVariable = pSkateboardScreenParams->someInt;
    context.pReturnValue = pSkateboardScreenParams->pReturnValue;
  }

  init();
  draw();
}

static void onTick(StackBasedFsm_t* pFsm) {
  int32_t recvByteCount;
  char recvBuffer[NRF_MAX_PAYLOAD_SIZE + 1];

  recvByteCount = nrf24_recvPacket(recvBuffer);
  recvBuffer[recvByteCount] = '\0';

  if(recvByteCount != NRF_NO_DATA_AVAILABLE) {
    // blink();

    debugPrint("Received [");
    debugPrintDec(recvByteCount);
    debugPrint("]: ");

    for(int i = 0; i < recvByteCount; ++i) {
      debugPrintHex(recvBuffer[i]);
      debugPrint(" ");
    }

    debugPrintln("");
  }
}

// Always implement this as last function of your state file:

void skateboardScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  // This callback MUST be set:
  pState->onEnterState    = onEnter;

  // The following callbacks are all optional:

  // Button press callbacks:
  pState->onActionPress    = onActionPress;
  pState->onBackPress      = onBackPress;
  pState->onStartPress     = onStartPress;
  pState->onSelectPress    = onSelectPress;
  pState->onDirectionPress = onDirectionPress; // TODO: implement onDirectionRelease()

  // Button release callbacks
  pState->onActionRelease = onActionRelease;
  pState->onBackRelease   = onBackRelease;
  pState->onStartRelease  = onStartRelease;
  pState->onSelectRelease = onSelectRelease;

  // State callbacks:
  pState->onReenterState  = onReenter;
  pState->onLeaveState    = onLeaveState;
  pState->onTick          = onTick;
}

