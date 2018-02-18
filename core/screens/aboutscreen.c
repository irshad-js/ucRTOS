#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../../core/ucrtos.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"
#include "../SlotBasedMenu.h"

#include "aboutscreen.h"

static void draw() {
  displayClear(0x00, 0x00, 0x00);
  displayDrawText(CENTER, 0 + 0 * 18, "ucRTOS v0.1", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 0 + 1 * 18, "coon@c-base.org", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  char pBuildInfo[64];
  strcpy(pBuildInfo, "Last build: ");
  strcat(pBuildInfo, __DATE__);
  strcat(pBuildInfo, ", ");
  strcat(pBuildInfo, __TIME__);

  displayDrawText(CENTER, 0 + 2 * 18, pBuildInfo, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  uint32_t t = upTimeMs();
  uint32_t s = (t / 1000) % 60;
  uint32_t m = (t / (1000 * 60)) % 60;
  uint32_t h = (t / (1000 * 60 * 60)) % 24;
  uint32_t d = (t / (1000 * 60 * 60 * 24));

  char pSeconds[16];
  char pMinutes[16];
  char pHours[16];
  char pDays[16];
  char pUptime[64];
  itoa(s, pSeconds, 10);
  itoa(m, pMinutes, 10);
  itoa(h, pHours, 10);
  itoa(d, pDays, 10);

  strcpy(pUptime, "Uptime: ");

  strcat(pUptime, pDays);
  strcat(pUptime, " days, ");

  if (h < 10)
    strcat(pUptime, "0");

  strcat(pUptime, pHours);
  strcat(pUptime, ":");

  if (m < 10)
    strcat(pUptime, "0");

  strcat(pUptime, pMinutes);
  strcat(pUptime, ":");

  if (s < 10)
    strcat(pUptime, "0");

  strcat(pUptime, pSeconds);
  displayDrawText(CENTER, 0 + 3 * 18, pUptime, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("aboutScreen::onEnter()");

  draw();
}

static void onBackPress(StackBasedFsm_t* pFsm) {
  hal_printf("aboutScreen::onBackPress()");

  leaveState(pFsm);
}

static void onTick(StackBasedFsm_t* pFsm) {
  hal_printf("aboutScreen::onTick()");
  draw();
}

void aboutScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  pState->onEnterState = onEnter;
  pState->onBackPress  = onBackPress;
  pState->onTick       = onTick;
}
