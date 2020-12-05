#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"
#include "../SlotBasedMenu.h"

#include "magiclanternscreen.h"

static struct {
  SlotBasedMenu_t menu;
  uint32_t address;
  uint32_t size;
} context;

static void draw() {
  displayClear();

  displayDrawText(CENTER, 0 + 0 * 18, "Portable RAM dumper", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 0 + 1 * 18, "Press 'Back' button to exit", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  menuDraw(&context.menu);

  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("magiclantern::onEnter()\n");

  context.address = 0;
  context.size = 0;

  menuInit(&context.menu, pFsm, 3, 85);
  menuAddHexValueSlot(&context.menu, "Address", &context.address);
  menuAddHexValueSlot(&context.menu, "Size", &context.size);
  menuAddTransitSlot(&context.menu, "read", 0);

  draw();
}

static void onAction(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("magiclantern::onAction(); pressed=%d\n", pressed);

  if (pressed) {
    menuAction(&context.menu, 0);
    draw();
  }
}

static void onBack(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("magiclantern::onBack(); pressed=%d\n");

  if (pressed) {
    MenuSlot_t* pSlot = &context.menu.pSlot[context.menu.cursorPos];

    if (pSlot->type == HEXVALUE_SLOT && pSlot->hexValueSlot.inEditMode) {
      menuBack(&context.menu);
      draw();
    }
    else
      leaveState(pFsm);
  }
}

static void onStart(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("magiclantern::onStartPress()\n");

  // This function is called, when the player presses or releases the Start button on the game pad.
}

static void onSelect(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("magiclantern::onSelectPress()\n");

  // This function is called, when the player presses or releases the Select button on the game pad.
}

static void onDirection(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("magiclantern::onDirectionPress()\n");

  if (south)
    menuMoveCursorDown(&context.menu);

  if (north)
    menuMoveCursorUp(&context.menu);

  if (west)
    menuMoveCursorLeft(&context.menu);

  if (east)
    menuMoveCursorRight(&context.menu);

  draw();
}

static void onReenter(StackBasedFsm_t* pFsm) {
  hal_printf("magiclantern::onReenter()\n");

  // This function is called, when the user pressed the back button on a higher state and lands back in this
  // state again.
}

static void onLeaveState(StackBasedFsm_t* pFsm) {
  hal_printf("magiclantern::onLeaveState()\n");
}

static void onTick(StackBasedFsm_t* pFsm) {
  // hal_printf("magiclantern::onTick()");

  // This function is called frequently as fast as possible, as long as this state is active. This state can be
  // used for time critical applications like animations, audio playback or advanced user interaction.
}

// Always implement this as last function of your state file:

void magicLanternScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  // This callback MUST be set:
  pState->onEnterState    = onEnter;

  // The following callbacks are all optional:

  // Button press callbacks:
  pState->onAction    = onAction;
  pState->onBack      = onBack;
  pState->onStart     = onStart;
  pState->onSelect    = onSelect;
  pState->onDirection = onDirection;

  // State callbacks:
  pState->onReenterState  = onReenter;
  pState->onLeaveState    = onLeaveState;
  pState->onTick          = onTick;
}
