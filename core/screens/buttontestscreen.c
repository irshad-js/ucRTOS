#include <string.h>

#include "../../core/ucrtos.h"
#include "../display.h"

#include "buttontestscreen.h"

static struct {
  bool selectIsPressed;
  bool startIsPressed;
} context;

static void draw() {
  // hal_printf("buttonTestScreen::draw()");

  InputDeviceStates_t state = getInputDeviceState();

  displayClear();
  displayDrawText(CENTER, 0  * 18, "Button Test",                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 1  * 18 , "Press Start + Select to leave",  0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  char numBuf[32];
  itoa(state.West, numBuf, 10);
  displayDrawText(0,      3  * 18, "Left: ",                  0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    3  * 18, numBuf,                    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(state.East, numBuf, 10);
  displayDrawText(0,      4 * 18, "Right: ",                  0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    4 * 18, numBuf,                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(state.North, numBuf, 10);
  displayDrawText(0,      5 * 18, "Up: ",                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    5 * 18, numBuf,                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(state.South, numBuf, 10);
  displayDrawText(0,      6 * 18, "Down: ",                   0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    6 * 18, numBuf,                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(state.Action, numBuf, 10);
  displayDrawText(0,      7 * 18, "Action: ",                 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    7 * 18, numBuf,                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(state.Back, numBuf, 10);
  displayDrawText(0,      8 * 18, "Back: ",                   0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    8 * 18, numBuf,                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(state.Start, numBuf, 10);
  displayDrawText(0,      9 * 18, "Start: ",                  0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    9 * 18, numBuf,                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(state.Select, numBuf, 10);
  displayDrawText(0,      10 * 18, "Select: ",                0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    10 * 18, numBuf,                    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDraw();
}

static void onAction(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("buttonTestScreen::onAction; pressed=%d\n", pressed);

  draw();
}

static void onBack(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("buttonTestScreen::onBack; pressed=%d\n", pressed);

  draw();
}

static void onStart(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("buttonTestScreen::onStart; pressed=%d\n", pressed);

  if (pressed) {
    context.startIsPressed = true;

    draw();

    if (context.startIsPressed && context.selectIsPressed)
      leaveState(pFsm);
  }
  else
    context.startIsPressed = false;
}

static void onSelect(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("buttonTestScreen::onSelect; pressed=%d\n", pressed);

  if (pressed) {
    context.selectIsPressed = true;

    draw();

    if (context.startIsPressed && context.selectIsPressed)
      leaveState(pFsm);
  }
  else
    context.selectIsPressed = false;
}
static void onDirection(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("buttonTestScreen::onDirection; south=%d, north=%d, west=%d, east=%d", south, north, west, east);

  draw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("buttonTestScreen::onEnter()");
  memset(&context, 0, sizeof(context));

  draw();
}

static void onTick(StackBasedFsm_t* pFsm) {
  if (upTimeMs() % 250 == 0)
    draw();
}

void buttonTestScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  pState->onEnterState  = onEnter;
  pState->onAction      = onAction;
  pState->onBack        = onBack;
  pState->onStart       = onStart;
  pState->onSelect      = onSelect;
  pState->onDirection   = onDirection;
  pState->onTick        = onTick;
}

