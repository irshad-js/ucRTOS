#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"

#include "livemodescreen.h"

static struct {
  int dummy;
} context;

static void draw() {
  displayClear();

  displayDrawText(CENTER, 0 + 0 * 18, "Live Mode screen", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 0 + 1 * 18, "TODO: implement RS485 processing", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("example::onEnter()");
    
  draw();
}

static void onActionPress(StackBasedFsm_t* pFsm) {
  hal_printf("example::onActionPress()");
}

static void onBackPress(StackBasedFsm_t* pFsm) {
  hal_printf("example::onBackPress()");

  leaveState(pFsm);
}

static void onStartPress(StackBasedFsm_t* pFsm) {
  hal_printf("example::onStartPress()");  
}

static void onSelectPress(StackBasedFsm_t* pFsm) {
  hal_printf("example::onSelectPress()");
}

static void onDirectionPress(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("example::onDirectionPress()");  
}

static void onActionRelease(StackBasedFsm_t* pFsm) {
  hal_printf("example::onActionRelease()"); 
}

static void onBackRelease(StackBasedFsm_t* pFsm) {
  hal_printf("example::onBackRelease()");  
}

static void onStartRelease(StackBasedFsm_t* pFsm) {
  hal_printf("example::onStartRelease()");  
}

static void onSelectRelease(StackBasedFsm_t* pFsm) {
  hal_printf("example::onSelectRelease()");  
}

static void onReenter(StackBasedFsm_t* pFsm) {
  hal_printf("example::onReenter()");  
}

static void onLeaveState(StackBasedFsm_t* pFsm) {
  hal_printf("example::onLeaveState()");
}

static void onTick(StackBasedFsm_t* pFsm) {
}

// Always implement this as last function of your state file:

void liveModeScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  // This callback MUST be set:
  pState->onEnterState    = onEnter;

  // The following callbacks are all optional:

  // Button press callbacks:
  pState->onActionPress    = onActionPress;
  pState->onBackPress      = onBackPress;
  pState->onStartPress     = onStartPress;
  pState->onSelectPress    = onSelectPress;
  pState->onDirectionPress = onDirectionPress;

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
