#include <string.h>

#include "../../core/ucrtos.h"
#include "../display.h"

#include "buttontestscreen.h"

static struct {
  int numActionPresses;
  int numBackPresses;
  int numStartPresses;
  int numSelectPresses;
  int numLeftPresses;
  int numRightPresses;
  int numDownPresses;
  int numUpPresses;

  bool selectIsPressed;
  bool startIsPressed;
} context;

static void draw() {
  hal_printf("buttonTestScreen::draw()");

  displayClear();
  displayDrawText(CENTER, 0  * 18, "Button Test",                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 1  * 18 , "Press Start + Select to leave",  0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  char numBuf[32];
  itoa(context.numLeftPresses, numBuf, 10);
  displayDrawText(0,      3  * 18, "Left: ",                  0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    3  * 18, numBuf,                    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(context.numRightPresses, numBuf, 10);
  displayDrawText(0,      4 * 18, "Right: ",                  0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    4 * 18, numBuf,                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(context.numUpPresses, numBuf, 10);
  displayDrawText(0,      5 * 18, "Up: ",                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    5 * 18, numBuf,                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(context.numDownPresses, numBuf, 10);
  displayDrawText(0,      6 * 18, "Down: ",                   0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    6 * 18, numBuf,                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(context.numActionPresses, numBuf, 10);
  displayDrawText(0,      7 * 18, "Action: ",                 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    7 * 18, numBuf,                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(context.numBackPresses, numBuf, 10);
  displayDrawText(0,      8 * 18, "Back: ",                   0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    8 * 18, numBuf,                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(context.numStartPresses, numBuf, 10);
  displayDrawText(0,      9 * 18, "Start: ",                  0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    9 * 18, numBuf,                     0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  itoa(context.numSelectPresses, numBuf, 10);
  displayDrawText(0,      10 * 18, "Select: ",                0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(100,    10 * 18, numBuf,                    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDraw();
}

static void onActionPress(StackBasedFsm_t* pFsm) {
  hal_printf("buttonTestScreen::onActionPress()");

  context.numActionPresses++;
  draw();
}

static void onBackPress(StackBasedFsm_t* pFsm) {
  hal_printf("buttonTestScreen::onBackPress()");

  context.numBackPresses++;
  draw();
}

static void onStartPress(StackBasedFsm_t* pFsm) {
  hal_printf("buttonTestScreen::onStartPress()");

  context.startIsPressed = true;
  context.numStartPresses++;

  draw();

  if (context.startIsPressed && context.selectIsPressed)
    leaveState(pFsm);
}

static void onStartRelease(StackBasedFsm_t* pFsm) {
  hal_printf("example::onBackRelease()");

  context.startIsPressed = false;
}

static void onSelectPress(StackBasedFsm_t* pFsm) {
  hal_printf("buttonTestScreen::onSelectPress()");

  context.selectIsPressed = true;
  context.numSelectPresses++;

  draw();

  if (context.startIsPressed && context.selectIsPressed)
    leaveState(pFsm);
}

static void onSelectRelease(StackBasedFsm_t* pFsm) {
  hal_printf("example::onSelectRelease()");

  context.selectIsPressed = false;
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("buttonTestScreen::onEnter()");
  memset(&context, 0, sizeof(context));

  draw();
}

static void onTick(StackBasedFsm_t* pFsm) {

}

static void onDirectionPress(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("buttonTestScreen::onDirection()");

  if (south)
    context.numDownPresses++;

  if (north)
    context.numUpPresses++;

  if (west)
    context.numLeftPresses++;

  if (east)
    context.numRightPresses++;

  draw();
}

void buttonTestScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  pState->onActionPress = onActionPress;
  pState->onBackPress = onBackPress;
  pState->onStartPress = onStartPress;
  pState->onStartRelease = onStartRelease;
  pState->onSelectPress = onSelectPress;
  pState->onSelectRelease = onSelectRelease;
  pState->onEnterState = onEnter;
  pState->onTick = onTick;
  pState->onDirectionPress = onDirectionPress;
}

