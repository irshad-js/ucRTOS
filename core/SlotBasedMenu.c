#include <stdint.h>
#include <string.h>
#include "SlotBasedMenu.h"
#include "images.h"
#include "display.h"
#include "../lib/colorprint/colorprint.h"

void hal_strcpy_s(char* pDst, int maxSize, const char* pSrc);

void menuInit(SlotBasedMenu_t* pSbm, StackBasedFsm_t* pFsm, int16_t xPos, int16_t yPos) {
  pSbm->xPos = xPos;
  pSbm->yPos = yPos;
  pSbm->cursorPos = 0;
  pSbm->numSlots = 0;
  pSbm->pFsm = pFsm;

  memset(pSbm->pSlot, 0, sizeof(pSbm->pSlot));
}

void menuAction(SlotBasedMenu_t* pSbm, void* pArgs) {
  switch (pSbm->pSlot[pSbm->cursorPos].type) {
    case TRANSIT_SLOT: {
      fsmPush(pSbm->pFsm, pSbm->pSlot[pSbm->cursorPos].transitSlot.pNextStateTransitionFunc, pArgs);
      break;
    }

    default: {
      hal_printfError("Error: invalid menu type!\n");
      break;
    }
  }
}

void menuBack(SlotBasedMenu_t* pSbm) {
  switch (pSbm->pSlot[pSbm->cursorPos].type) {
    case TRANSIT_SLOT: {
      fsmPop(pSbm->pFsm);
      break;
    }

    default: {
      hal_printfError("Error: invalid menu type!\n");
      break;
    }
  }
}

static void menuDrawCursor(SlotBasedMenu_t* pSbm) {
  displayDrawImage(pSbm->xPos, pSbm->yPos + 18 * pSbm->cursorPos, _pCursorImg);
}

void menuAddTransitSlot(SlotBasedMenu_t* pSbm, const char* pLabel, TransitionFunc pFunc) {
  if (pSbm->numSlots >= MENU_MAX_SLOTS)
    return;

  pSbm->pSlot[pSbm->numSlots].type = TRANSIT_SLOT;
  hal_strcpy_s(pSbm->pSlot[pSbm->numSlots].transitSlot.pLabel, MAX_MENU_ITEM_CHARS, pLabel);
  pSbm->pSlot[pSbm->numSlots].transitSlot.pNextStateTransitionFunc = pFunc;
  pSbm->numSlots++;
}

void menuAddHexValueSlot(SlotBasedMenu_t* pSbm, const char* pLabel, uint32_t* pValue) {
  if (pSbm->numSlots >= MENU_MAX_SLOTS)
    return;

  MenuSlot_t* pSlot = &pSbm->pSlot[pSbm->numSlots];
  pSlot->type = HEXVALUE_SLOT;
  pSlot->hexValueSlot.inEditMode = false;
  pSlot->hexValueSlot.digitPos = 0;
  hal_strcpy_s(pSlot->hexValueSlot.pLabel, MAX_MENU_ITEM_CHARS, pLabel);
  pSlot->hexValueSlot.pValue = pValue;

  pSbm->numSlots++;
}

void menuDraw(SlotBasedMenu_t* pSbm) {
  for (int i = 0; i < pSbm->numSlots; i++) {
    uint16_t x = (uint16_t)(pSbm->xPos + 28);
    uint16_t y = (uint16_t)(pSbm->yPos - 5 + 18 * i);

    switch (pSbm->pSlot[i].type) {
      case TRANSIT_SLOT: {
        uint8_t c = pSbm->pSlot[i].transitSlot.pNextStateTransitionFunc ? 0xFF : 0xAA;
        displayDrawText(x, y, pSbm->pSlot[i].transitSlot.pLabel, c, c, c, 0x00, 0x00, 0x00);
        break;
      }

      case HEXVALUE_SLOT: {
        HexValueSlot_t* pSlot = &pSbm->pSlot[i].hexValueSlot;

        uint8_t cl = pSlot->inEditMode ? 0xFF : 0xAA;
        displayDrawText(x, y, pSlot->pLabel, cl, cl, cl, 0x00, 0x00, 0x00);
        x += 8 * strlen(pSlot->pLabel);
        displayDrawText(x, y, ": ", cl, cl, cl, 0x00, 0x00, 0x00);
        x += 8 * 2;

        for (int j = 0; j < 8; ++j) {
          uint8_t cd = pSlot->inEditMode && j == pSlot->digitPos ? 0xFF : 0xAA;

          displayDrawText(x, y, "X", cd, cd, cd, 0x00, 0x00, 0x00);

          x += 8;
        }

        break;
      }

      default: {
        hal_printfError("Error: invalid menu type!\n");
        break;
      }
    }
  }

  menuDrawCursor(pSbm);
}

void menuMoveCursorUp(SlotBasedMenu_t* pSbm) {
  switch (pSbm->pSlot[pSbm->cursorPos].type) {
    case TRANSIT_SLOT: {
      if (pSbm->cursorPos > 0)
        pSbm->cursorPos--;
      break;
    }

    case HEXVALUE_SLOT: {
      if (!pSbm->pSlot[pSbm->cursorPos].hexValueSlot.inEditMode && pSbm->cursorPos > 0)
        pSbm->cursorPos--;
      break;
    }

    default: {
      hal_printfError("Error: invalid menu type!\n");
      break;
    }
  }
}

void menuMoveCursorDown(SlotBasedMenu_t* pSbm) {
  switch (pSbm->pSlot[pSbm->cursorPos].type) {
    case TRANSIT_SLOT: {
      if (pSbm->cursorPos + 1 < pSbm->numSlots)
        pSbm->cursorPos++;
      break;
    }

    case HEXVALUE_SLOT: {
      if (!pSbm->pSlot[pSbm->cursorPos].hexValueSlot.inEditMode && pSbm->cursorPos + 1 < pSbm->numSlots)
        pSbm->cursorPos++;
      break;
    }

    default: {
      hal_printfError("Error: invalid menu type!\n");
      break;
    }
  }
}
