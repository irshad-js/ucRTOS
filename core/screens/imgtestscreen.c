#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"
#include "../images.h"

#include "imgtestscreen.h"

static void draw() {
  displayClear();

  displayDrawImage(30, 50, _pNesGamepadImg);
  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("imgTestScreen::onEnter()");

  draw();
}

static void onBack(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("imgTestScreen::onBackPress()");

  if (pressed)
    leaveState(pFsm);
}

void imgTestScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  pState->onEnterState = onEnter;
  pState->onBack       = onBack;
}

