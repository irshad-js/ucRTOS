#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"

#include "magiclanternscreen.h"

static struct {
  // put your local variables here. This can also be used to return a value to the previous state:

  int someLocalVariable;
  int* pReturnValue;

} context;

static void draw() {
  displayClear();

  displayDrawText(CENTER, 0 + 0 * 18, "Magic Lantern screen", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 0 + 1 * 18, "Press 'Back' button to exit", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("magiclantern::onEnter()\n");

  // This function is called, when the state is entered for the first time.
  // It must be defined in every state and is used for initialization.
  // This can also be used for passing a return value to the previous state.

  MagicLanternScreenParams* pMlParams = (MagicLanternScreenParams*)pParams;
  if (!pMlParams)
    hal_printfWarning("Param is nullptr\n");
  else {
    context.someLocalVariable = pMlParams->someInt;
    context.pReturnValue = pMlParams->pReturnValue;
  }

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

  // This function is called, if the user presses or releases one of the direction buttons on the game pad.
}

static void onReenter(StackBasedFsm_t* pFsm) {
  hal_printf("magiclantern::onReenter()\n");

  // This function is called, when the user pressed the back button on a higher state and lands back in this
  // state again.
}

static void onLeaveState(StackBasedFsm_t* pFsm) {
  hal_printf("magiclantern::onLeaveState()\n");

  // This function is called, when the user leaves the current state. Either by going to a next state or by
  // going back to the previous state.

  if (!context.pReturnValue) {
    hal_printfWarning("Cannot return value. pReturnValue is null\n");
    return;
  }

  *context.pReturnValue = 42;
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
