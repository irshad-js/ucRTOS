#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"
#include "nrf24l01p.h"

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

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("skateboard::onEnter()");

  SkateboardScreenParams* pSkateboardScreenParams = (SkateboardScreenParams*)pParams;
  if (!pParams)
    hal_printfWarning("Param is nullptr");
  else {
    context.someLocalVariable = pSkateboardScreenParams->someInt;
    context.pReturnValue = pSkateboardScreenParams->pReturnValue;
  }

  draw();
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

static void onTick(StackBasedFsm_t* pFsm) {
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

