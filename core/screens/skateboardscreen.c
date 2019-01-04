#include <stdio.h>
#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"
#include "nrf24l01p.h"
#include "skateboardscreen.h"

static struct {
  bool buttonIsPressed;
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

static void onAction(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("skateboard::onActionPress; pressed=%d\n", pressed);

  context.buttonIsPressed = pressed;
}

static void onBack(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("skateboard::onBack; pressed=%d\n", pressed);
}

static void onStart(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("skateboard::onStart; pressed=%d\n", pressed);
}

static void onSelect(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("skateboard::onSelect; pressed=%d\n", pressed);

  if (pressed)
    leaveState(pFsm);
}

static void onDirection(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("skateboard::onDirection\n");
}

static void onReenter(StackBasedFsm_t* pFsm) {
  hal_printf("skateboard::onReenter\n");
}

static void onLeaveState(StackBasedFsm_t* pFsm) {
  hal_printf("skateboard::onLeaveState\n");

  if (!context.pReturnValue) {
    hal_printfWarning("Cannot return value. pReturnValue is null\n");
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

  while (time_var1)
    ;
}

static void blink() {
  // TODO: add API function for controlling debug LED

  //  GPIO_SetBits(GPIOD, GPIO_Pin_12);
  Delay(50);

  //  GPIO_ResetBits(GPIOD, GPIO_Pin_12);
  Delay(50);
}

static void sendUartText(const char* pText) {
  hal_printf(pText);
}

static void debugPrint(const char* pText) {
  sendUartText(pText);
}

static void debugPrintln(const char* pText) {
  hal_printf("%s\n", pText);
}

static void debugPrintDec(int n) {
  char pNum[32];
  snprintf(pNum, sizeof(pNum), "%d", n);

  sendUartText(pNum);
}

static void debugPrintHex(int n) {
  char pHex[32];
  snprintf(pHex, sizeof(pHex), "0x%02X", n);

  sendUartText(pHex);
}

static void printFullConfig() {
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

static void setup_nrf24() {
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
  setup_nrf24();
}

// -----------------------------------------

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("skateboard::onEnter\n");

  SkateboardScreenParams* pSkateboardScreenParams = (SkateboardScreenParams*)pParams;
  if (!pParams)
    hal_printfWarning("skateboard::onEnter; pParams is nullptr");

  context.buttonIsPressed = false;

  init();
  draw();
}

static void onTick(StackBasedFsm_t* pFsm) {
/*
  int32_t recvByteCount = NRF_NO_DATA_AVAILABLE;
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
*/

  uint8_t pData[] = {
    0xC0, 0xFE, 0xFE, 0x00, 0x42, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };

  if (context.buttonIsPressed)
    pData[4] = 0x42;
  else
    pData[4] = 0x23;

  nrf24_sendPacket(pData, 16, false);
}

// Always implement this as last function of your state file:

void skateboardScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  pState->onEnterState    = onEnter;
  pState->onAction        = onAction;
  pState->onBack          = onBack;
  pState->onStart         = onStart;
  pState->onSelect        = onSelect;
  pState->onDirection     = onDirection;
  pState->onReenterState  = onReenter;
  pState->onLeaveState    = onLeaveState;
  pState->onTick          = onTick;
}

