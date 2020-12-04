#include "../../lib/colorprint/colorprint.h"
#include "../../lib/LockFreeFifo/LockFreeFifo.h"
#include "../../lib/mystdio/mystdio.h"
#include "../../core/ucrtos.h"
#include "../display.h"
#include "../SlotBasedMenu.h"

#include "_examplescreen.h"
#include "aboutscreen.h"
#include "imgtestscreen.h"
#include "skateboardscreen.h"
#include "floppyorgelscreen.h"
#include "buttontestscreen.h"
#include "sdcardscreen.h"
#include "magiclanternscreen.h"
#include "mainmenuscreen.h"

static struct {
  LockFreeFIFO_t fifoDebugPort;
  SlotBasedMenu_t menu;

} _context;

static void draw() {
  uint32_t start = upTimeMs();

  displayClear();
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

  hal_rs485init(&_context.fifoDebugPort); // TODO: move to live mode state?
  menuInit(&_context.menu, pFsm, 3, 45);

  // CHECKME: adding the following menu entry messes up the LUT!?:
  menuAddTransitSlot(&_context.menu, "Floppy Orgel", floppyOrgelScreen);
  menuAddTransitSlot(&_context.menu, "Magic Lantern", magicLanternScreen);
  menuAddTransitSlot(&_context.menu, "Skateboard", skateboardScreen);
  menuAddTransitSlot(&_context.menu, "SD Card", sdCardScreen);
  menuAddTransitSlot(&_context.menu, "Button Test", buttonTestScreen);
  menuAddTransitSlot(&_context.menu, "Example", exampleScreen);
  menuAddTransitSlot(&_context.menu, "Image Test", imgTestScreen);
  menuAddTransitSlot(&_context.menu, "About", aboutScreen);

  draw();
}

static void onAction(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("mainMenuScreen::onAction; pressed=%d\n", pressed);

  menuAction(&_context.menu, &_context.fifoDebugPort);
}

static void onBack(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("mainMenuScreen::onBack; pressed=%d\n", pressed);

  menuBack(&_context.menu);
}

static void onStart(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("mainMenuScreen::onStart; pressed=%d\n", pressed);

  // This function is called, when the player presses the Start button on the Gamepad.
}

static void onSelect(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("mainMenuScreen::onSelect; pressed=%d\n", pressed);

  // This function is called, when the player presses the Select button on the Gamepad.
}

static void onDirection(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("mainMenuScreen::onDirection;\n");

  if (south)
    menuMoveCursorDown(&_context.menu);

  if (north)
    menuMoveCursorUp(&_context.menu);

  draw();
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
  pState->onEnterState   = onEnter;
  pState->onAction       = onAction;
  pState->onBack         = onBack;
  pState->onStart        = onStart;
  pState->onSelect       = onSelect;
  pState->onDirection    = onDirection;
  pState->onReenterState = onReenter;
  pState->onLeaveState   = onLeaveState;
  pState->onTick         = onTick;
}

