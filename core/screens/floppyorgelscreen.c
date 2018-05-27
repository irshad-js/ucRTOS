#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"
#include "../SlotBasedMenu.h"

#include "livemodescreen.h"
#include "playermenuscreen.h"
#include "floppyorgelscreen.h"

static struct {
  SlotBasedMenu_t menu;

} context;

static void draw() {
  displayClear();
  displayDrawText(CENTER, 0 + 0 * 18, "Floppy Orgel Menu", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);  
  menuDraw(&context.menu);
  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("FloppyOrgelScreen::onEnter()");

  userMenuInit(&context.menu, pFsm, 3, 85);
  menuAddSlot(&context.menu, "Live Mode", liveModeScreen);
  menuAddSlot(&context.menu, "MIDI Player", playerMenuScreen);
      
  draw();
}

static void onActionPress(StackBasedFsm_t* pFsm) {
  hal_printf("FloppyOrgelScreen::onActionPress()");

  userMenuTransitToSelectedSlot(&context.menu, 0);
}

static void onBackPress(StackBasedFsm_t* pFsm) {
  hal_printf("FloppyOrgelScreen::onBackPress()");

  userMenuTransitBack(&context.menu);
}

static void onDirectionPress(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("FloppyOrgelScreen::onDirectionPress()");

  if (south)
    menuMoveCursorDown(&context.menu);

  if (north)
    menuMoveCursorUp(&context.menu);

  draw();
}

static void onReenter(StackBasedFsm_t* pFsm) {
  hal_printf("FloppyOrgelScreen::onReenter()");

  draw();
}

// Always implement this as last function of your state file:

void floppyOrgelScreen(StackBasedFsm_t* pFsm, FsmState* pState) {  
  pState->onEnterState     = onEnter;  
  pState->onActionPress    = onActionPress;
  pState->onBackPress      = onBackPress;
  pState->onDirectionPress = onDirectionPress;
  pState->onReenterState   = onReenter;  
}
