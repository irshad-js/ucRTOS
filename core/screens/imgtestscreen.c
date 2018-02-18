#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"

#include "imgtestscreen.h"

static void draw() {
  displayClear();

  displayDrawText(CENTER, 0 + 0 * 18, "Img test screen", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 0 + 1 * 18, "Press 'Back' button to exit", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("imgTestScreen::onEnter()");

  draw();
}

static void onBackPress(StackBasedFsm_t* pFsm) {
  hal_printf("imgTestScreen::onBackPress()");

  leaveState(pFsm);
}

void imgTestScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  pState->onEnterState    = onEnter;
  pState->onBackPress     = onBackPress;
}
