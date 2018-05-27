#include "../../lib/colorprint/colorprint.h"
#include "../../lib/mystdio/mystdio.h"
#include "../../core/ucrtos.h"
#include "../display.h"
#include "../SlotBasedMenu.h"

#include "_examplescreen.h"
#include "aboutscreen.h"
#include "imgtestscreen.h"
#include "skateboardscreen.h"
#include "floppyorgelscreen.h"
#include "mainmenuscreen.h"

static struct {
  SlotBasedMenu_t menu;

} _context;

static void draw() {
  uint32_t start = upTimeMs();

  displayClear(0x00, 0x00, 0x00);
  hal_printf("displayClear: %d ms\n", upTimeMs() - start);

  displayDrawText(CENTER, 0 + 0 * 18, "Welcome to ucRTOS", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  hal_printf("text 1: %d ms\n", upTimeMs() - start);

  displayDrawText(CENTER, 0 + 1 * 18, "This is the main menu", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  hal_printf("text 2: %d ms\n", upTimeMs() - start);

  menuDraw(&_context.menu);
  hal_printf("menuDraw: %d ms\n", upTimeMs() - start);

  displayDraw();
  hal_printf("displayDraw: %d ms\n", upTimeMs() - start);
  hal_printf("==================\n");
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("mainMenuScreen::onEnter()\n");

  userMenuInit(&_context.menu, pFsm, 3, 45);
  menuAddSlot(&_context.menu, "Floppy Orgel", floppyOrgelScreen);
  menuAddSlot(&_context.menu, "Skateboard", NULL);  
  menuAddSlot(&_context.menu, "Button Test", NULL);
  menuAddSlot(&_context.menu, "Example", exampleScreen);
  menuAddSlot(&_context.menu, "Image Test", imgTestScreen);
  menuAddSlot(&_context.menu, "About", aboutScreen);

  draw();
}

static void onActionPress(StackBasedFsm_t* pFsm) {
  hal_printf("mainMenuScreen::onActionPress\n");

  userMenuTransitToSelectedSlot(&_context.menu, 0);
}

static void onBackPress(StackBasedFsm_t* pFsm) {
  hal_printf("mainMenuScreen::onBackPress\n");

  userMenuTransitBack(&_context.menu);
}

static void onStartPress(StackBasedFsm_t* pFsm) {
  hal_printf("mainMenuScreen::onStartPress\n");

  // This function is called, when the player presses the Start button on the Gamepad.
}

static void onSelectPress(StackBasedFsm_t* pFsm) {
  hal_printf("mainMenuScreen::onSelectPress\n");

  // This function is called, when the player presses the Select button on the Gamepad.
}

static void onDirectionPress(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("mainMenuScreen::onDirectionPress\n");

  if (south)
    menuMoveCursorDown(&_context.menu);

  if (north)
    menuMoveCursorUp(&_context.menu);

  draw();
}

static void onActionRelease(StackBasedFsm_t* pFsm) {
  hal_printf("mainMenuScreen::onActionRelease\n");

  // This function is called, when the player releases the action button on the game pad.
  // On the NES game pad this is the 'A' button.
}

static void onBackRelease(StackBasedFsm_t* pFsm) {
  hal_printf("mainMenuScreen::onBackRelease\n");

  // This function is called, when the player releases the back button on the Gamepad.
  // On the NES game pad this is the 'B' button.
}

static void onStartRelease(StackBasedFsm_t* pFsm) {
  hal_printf("mainMenuScreen::onStartRelease\n");

  // This function is called, when the player releases the Start button on the Gamepad.
}

static void onSelectRelease(StackBasedFsm_t* pFsm) {
  hal_printf("mainMenuScreen::onSelectRelease\n");

  // This function is called, when the player releases the Select button on the Gamepad.
}

static void onReenter(StackBasedFsm_t* pFsm) {
  hal_printf("mainMenuScreen::onReenter\n");

  draw();
}

static void onLeaveState(StackBasedFsm_t* pFsm) {
  hal_printf("mainMenuScreen::onLeaveState\n");

  // This function is called, when the user leaves the current state. Either by going to a next state or by
  // going back to the previous state.
}

static void onTick(StackBasedFsm_t* pFsm) {
  // hal_printf("mainMenuScreen::onTick()");

  // This function is called frequently as fast as possible, as long as this state is active. This state can be
  // used for time critical applications like animations, audio playback or advanced user interaction.
}

// Always implement this as last function of your state file:

void mainMenuScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
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

