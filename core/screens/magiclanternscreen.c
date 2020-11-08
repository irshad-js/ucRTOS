#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"
#include "../SlotBasedMenu.h"

#include "magiclanternscreen.h"

static struct {
  SlotBasedMenu_t menu;
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

  userMenuInit(&context.menu, pFsm, 3, 85);
  menuAddSlot(&context.menu, "Address: 00000000", 0);
  menuAddSlot(&context.menu, "Size:    00000000", 0);
  menuAddSlot(&context.menu, "read", 0);

  draw();
}

static void onAction(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("magiclantern::onActionPress()\n");

  // This function is called, when the player presses or releases the action button on the game pad.
  // On the NES game pad this is the 'A' button.
}

static void onBack(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("magiclantern::onBackPress()\n");

  // This function is called, when the player presses or releases the back button on the game pad.
  // On the NES game pad this is the 'B' button.

  // In most cases you want to go to the previous screen. This is done by calling the 'leaveState()' function:
  leaveState(pFsm);
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
