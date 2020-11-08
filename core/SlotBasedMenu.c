#include <stdint.h>
#include <string.h>
#include "SlotBasedMenu.h"
#include "images.h"
#include "display.h"

void hal_strcpy_s(char* dst, int maxSize, const char* src);

static void menuInit(SlotBasedMenu_t* pSbm, StackBasedFsm_t* pFsm, int16_t xPos, int16_t yPos) {
  pSbm->xPos = xPos;
  pSbm->yPos = yPos;
  pSbm->cursorPos = 0;
  pSbm->numSlots = 0;
  pSbm->pFsm = pFsm;

  memset(pSbm->slot, 0, sizeof(pSbm->slot));
}

void userMenuInit(SlotBasedMenu_t* pSbm, StackBasedFsm_t* pFsm, int16_t xPos, int16_t yPos) {
  menuInit(pSbm, pFsm, xPos, yPos);
}

void menuAction(SlotBasedMenu_t* pMenu, void* pArgs) {
  fsmPush(pMenu->pFsm, pMenu->slot[pMenu->cursorPos].pNextStateTransitionFunc, pArgs);
}

void menuBack(SlotBasedMenu_t* pMenu) {
  fsmPop(pMenu->pFsm);
}

static void menuDrawCursor(SlotBasedMenu_t* pSbm) {
  displayDrawImage(pSbm->xPos, pSbm->yPos + 18 * pSbm->cursorPos, _pCursorImg);
}

void menuAddSlot(SlotBasedMenu_t* pSbm, const char* label, TransitionFunc pFunc) {
  if (pSbm->numSlots >= MENU_MAX_SLOTS)
    return;

  hal_strcpy_s(pSbm->slot[pSbm->numSlots].pLabel, MAX_MENU_ITEM_CHARS, label);
  pSbm->slot[pSbm->numSlots].pNextStateTransitionFunc = pFunc;
  pSbm->numSlots++;
}

void menuDraw(SlotBasedMenu_t* sbm) {
  for (int i = 0; i < sbm->numSlots; i++) {
    uint16_t x = (uint16_t)(sbm->xPos + 28);
    uint16_t y = (uint16_t)(sbm->yPos - 5 + 18 * i);
    uint8_t c = sbm->slot[i].pNextStateTransitionFunc ? 0xFF : 0xAA;

    displayDrawText(x, y, sbm->slot[i].pLabel, c, c, c, 0x00, 0x00, 0x00);
  }

  menuDrawCursor(sbm);
}

void menuMoveCursorUp(SlotBasedMenu_t* sbm) {
  if (sbm->cursorPos > 0)
    sbm->cursorPos--;
}

void menuMoveCursorDown(SlotBasedMenu_t* sbm) {
  if (sbm->cursorPos + 1 < sbm->numSlots)
    sbm->cursorPos++;
}
