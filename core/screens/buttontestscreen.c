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

static void onActionPress(StackBasedFsm_t* pFsm) {
  hal_printf("buttonTestScreen::onActionPress()");

  draw();
}

static void onActionRelease(StackBasedFsm_t* pFsm) {
  hal_printf("buttonTestScreen::onActionRelease()");

  draw();
}

static void onBackPress(StackBasedFsm_t* pFsm) {
  hal_printf("buttonTestScreen::onBackPress()");

  draw();
}

static void onBackRelease(StackBasedFsm_t* pFsm) {
  hal_printf("buttonTestScreen::onBackRelease()");

  draw();
}

static void onStartPress(StackBasedFsm_t* pFsm) {
  hal_printf("buttonTestScreen::onStartPress()");

  context.startIsPressed = true;

  draw();

  if (context.startIsPressed && context.selectIsPressed)
    leaveState(pFsm);
}

static void onStartRelease(StackBasedFsm_t* pFsm) {
  hal_printf("buttonTestScreen::onStartRelease()");

  context.startIsPressed = false;
}

static void onSelectPress(StackBasedFsm_t* pFsm) {
  hal_printf("buttonTestScreen::onSelectPress()");

  context.selectIsPressed = true;

  draw();

  if (context.startIsPressed && context.selectIsPressed)
    leaveState(pFsm);
}

static void onSelectRelease(StackBasedFsm_t* pFsm) {
  hal_printf("buttonTestScreen::onSelectRelease()");

  context.selectIsPressed = false;
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
  pState->onActionPress = onActionPress;
  pState->onActionRelease = onActionRelease;
  pState->onBackPress = onBackPress;
  pState->onBackRelease = onBackRelease;
  pState->onStartPress = onStartPress;
  pState->onStartRelease = onStartRelease;
  pState->onSelectPress = onSelectPress;
  pState->onSelectRelease = onSelectRelease;
  pState->onEnterState = onEnter;
  pState->onTick = onTick;
}

