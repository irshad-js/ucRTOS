#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"

#include "_example.h"

//--------------------------------------------------------------------------------------------------------------
// Example screen implementation
// ============================
//
// This example shows, how to implement a screen.
//
// - Each screen has its own .c file, which consists of a collection of callbacks.
// - The onEnter() callback must be set. All other callbacks are optional.
// - At the end of the file there is always an initialization function, which activates the state by setting
//   its callbacks.
// - The context struct can be used to store local variables. If this state gets called multiple times in a
//   state chain, an array of contexts can be used, to get an own context for each state.
//--------------------------------------------------------------------------------------------------------------

static struct {
  // put your local variables here. This can also be used to return a value to the previous state:

  int someLocalVariable;
  int* pReturnValue;

} context;

static void draw() {
  displayClear();

  displayDrawText(CENTER, 0 + 0 * 18, "Example screen", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 0 + 1 * 18, "Press 'Back' button to exit", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("example::onEnter()");

  // This function is called, when the state is entered for the first time.
  // It must be defined in every state and is used for initialization.
  // This can also be used for passing a return value to the previous state.

  ExampleScreenParams* pExampleParams = (ExampleScreenParams*)pParams;
  if (!pExampleParams)
    hal_printfWarning("Param is nullptr");
  else {
    context.someLocalVariable = pExampleParams->someInt;
    context.pReturnValue = pExampleParams->pReturnValue;
  }

  draw();
}

static void onActionPress(StackBasedFsm_t* pFsm) {
  hal_printf("example::onActionPress()");

  // This function is called, when the player presses the action button on the game pad.
  // On the NES game pad this is the 'A' button.
}

static void onBackPress(StackBasedFsm_t* pFsm) {
  hal_printf("example::onBackPress()");

  // This function is called, when the player presses the back button on the Gamepad.
  // On the NES game pad this is the 'B' button.

  // In most cases you want to go to the previous screen. This is done by calling the 'leaveState()' function:
  leaveState(pFsm);
}

static void onStartPress(StackBasedFsm_t* pFsm) {
  hal_printf("example::onStartPress()");

  // This function is called, when the player presses the Start button on the Gamepad.
}

static void onSelectPress(StackBasedFsm_t* pFsm) {
  hal_printf("example::onSelectPress()");

  // This function is called, when the player presses the Select button on the Gamepad.
}

static void onDirectionPress(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("example::onDirectionPress()");

  // This function is called, if the user presses one of the direction buttons on the game pad.
}

static void onActionRelease(StackBasedFsm_t* pFsm) {
  hal_printf("example::onActionRelease()");

  // This function is called, when the player releases the action button on the game pad.
  // On the NES game pad this is the 'A' button.
}

static void onBackRelease(StackBasedFsm_t* pFsm) {
  hal_printf("example::onBackRelease()");

  // This function is called, when the player releases the back button on the Gamepad.
  // On the NES game pad this is the 'B' button.
}

static void onStartRelease(StackBasedFsm_t* pFsm) {
  hal_printf("example::onStartRelease()");

  // This function is called, when the player releases the Start button on the Gamepad.
}

static void onSelectRelease(StackBasedFsm_t* pFsm) {
  hal_printf("example::onSelectRelease()");

  // This function is called, when the player releases the Select button on the Gamepad.
}

static void onReenter(StackBasedFsm_t* pFsm) {
  hal_printf("example::onReenter()");

  // This function is called, when the user pressed the back button on a higher state and lands back in this
  // state again.
}

static void onLeaveState(StackBasedFsm_t* pFsm) {
  hal_printf("example::onLeaveState()");

  // This function is called, when the user leaves the current state. Either by going to a next state or by
  // going back to the previous state.

  if (!context.pReturnValue) {
    hal_printfWarning("Cannot return value. pReturnValue is null");
    return;
  }

  *context.pReturnValue = 42;
}

static void onTick(StackBasedFsm_t* pFsm) {
  // hal_printf("example::onTick()");

  // This function is called frequently as fast as possible, as long as this state is active. This state can be
  // used for time critical applications like animations, audio playback or advanced user interaction.
}

// Always implement this as last function of your state file:

void exampleScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
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
