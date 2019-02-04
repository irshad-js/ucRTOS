#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"

#include "sdcardscreen.h"

static struct {
} context;

static void draw() {
  displayClear();

  displayDrawText(CENTER, 0 + 0 * 18, "SD Card Screen", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 0 + 1 * 18, "Press 'Back' button to exit", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("sdcard::onEnter()");

  draw();
}

static void onBack(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("example::onBackPress()");

  leaveState(pFsm);
}

// Always implement this as last function of your state file:

void sdCardScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  // This callback MUST be set:
  pState->onEnterState    = onEnter;

  // The following callbacks are all optional:

  // Button press callbacks:
  // pState->onAction    = onAction;
  pState->onBack      = onBack;
  // pState->onStart     = onStart;
  // pState->onSelect    = onSelect;
  // pState->onDirection = onDirection;

  // State callbacks:
  // pState->onReenterState  = onReenter;
  // pState->onLeaveState    = onLeaveState;
  // pState->onTick          = onTick;
}
