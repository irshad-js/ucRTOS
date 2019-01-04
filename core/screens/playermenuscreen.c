#include <stddef.h>
#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"

#include "playermenuscreen.h"
#include "playerscreen.h"

static void draw() {
  displayClear();

  displayDrawText(CENTER, 0 + 0 * 18, "Select a song and press 'A'", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 0 + 1 * 18, "Press 'Back' button to exit", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDrawText(CENTER, 0 + 3 * 18, "TODO: add menu entries", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("playerMenuScreen::onEnter\n");

  draw();
}

static void onAction(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("playerMenuScreen::onActionPress\n");

  if (pressed)
    fsmPush(pFsm, playerScreen, NULL); // TODO: provide song to play as argument
}

static void onBack(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("playerMenuScreen::onBackPress\n");

  if (pressed)
    leaveState(pFsm);
}

static void onStart(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("playerMenuScreen::onStartPress\n");

  // TODO: play MIDI
}

static void onDirection(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("playerMenuScreen::onDirectionPress\n");

  // This function is called, if the user presses one of the direction buttons on the game pad.
}

static void onReenter(StackBasedFsm_t* pFsm) {
  hal_printf("playerMenuScreen::onReenter\n");

  draw();
}

// Always implement this as last function of your state file:

void playerMenuScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  pState->onEnterState   = onEnter;
  pState->onAction       = onAction;
  pState->onBack         = onBack;
  pState->onStart        = onStart;
  pState->onDirection    = onDirection;
  pState->onReenterState = onReenter;
}

