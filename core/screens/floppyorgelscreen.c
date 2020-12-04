#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../../lib/LockFreeFifo/LockFreeFifo.h"
#include "../display.h"
#include "../SlotBasedMenu.h"

#include "livemodescreen.h"
#include "playermenuscreen.h"
#include "floppyorgelscreen.h"

static struct {
  LockFreeFIFO_t* pFifoDebugPort;
  SlotBasedMenu_t menu;

} context;

static void draw() {
  displayClear();
  displayDrawText(CENTER, 0 + 0 * 18, "Floppy Orgel Menu", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  menuDraw(&context.menu);
  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("FloppyOrgelScreen::onEnter()\n");

  context.pFifoDebugPort = (LockFreeFIFO_t*)pParams;

  menuInit(&context.menu, pFsm, 3, 85);
  menuAddTransitSlot(&context.menu, "Live Mode", liveModeScreen);
  menuAddTransitSlot(&context.menu, "MIDI Player", playerMenuScreen);

  draw();
}

static void onAction(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("FloppyOrgelScreen::onActionPress()\n");

  if (pressed)
    menuAction(&context.menu, context.pFifoDebugPort);
}

static void onBack(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("FloppyOrgelScreen::onBackPress()\n");

  if (pressed)
    menuBack(&context.menu);
}

static void onDirection(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("FloppyOrgelScreen::onDirectionPress()\n");

  if (south)
    menuMoveCursorDown(&context.menu);

  if (north)
    menuMoveCursorUp(&context.menu);

  draw();
}

static void onReenter(StackBasedFsm_t* pFsm) {
  hal_printf("FloppyOrgelScreen::onReenter()\n");

  draw();
}

// Always implement this as last function of your state file:

void floppyOrgelScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  pState->onEnterState   = onEnter;
  pState->onAction       = onAction;
  pState->onBack         = onBack;
  pState->onDirection    = onDirection;
  pState->onReenterState = onReenter;
}

