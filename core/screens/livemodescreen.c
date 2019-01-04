#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../../lib/LockFreeFifo/LockFreeFifo.h"
#include "../display.h"

#include "livemodescreen.h"

static struct {
  LockFreeFIFO_t* pFifoDebugPort;

} context;

static void draw() {
  displayClear();

  displayDrawText(CENTER, 0 + 0 * 18, "Live Mode screen", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 0 + 1 * 18, "TODO: implement RS485 processing", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("LiveModeScreen::onEnter()");

  context.pFifoDebugPort = (LockFreeFIFO_t*)pParams;

  displayClear();
  displayDrawText(CENTER, 0, "--- Live mode ---", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 18, "Now receiving MIDI-Data on debug port...", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  draw();
}

static void onBack(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("LiveModeScreen::onBackPress()");

  if (pressed)
    leaveState(pFsm);
}

static void onLeaveState(StackBasedFsm_t* pFsm) {
  hal_printf("LiveModeScreen::onLeaveState()");

  // TODO: stop all floppies
}

static void onTick(StackBasedFsm_t* pFsm) {
  // while (getRingBufferDistance(context.pFifoDebugPort) < RING_BUFFER_SIZE)

  while (ringBufferDataAvailable(context.pFifoDebugPort))
    hal_rs485Send(readFromRingBuffer(context.pFifoDebugPort));
}

// Always implement this as last function of your state file:

void liveModeScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  pState->onEnterState = onEnter;
  pState->onBack       = onBack;
  pState->onLeaveState = onLeaveState;
  pState->onTick       = onTick;
}

