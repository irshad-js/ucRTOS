#ifndef __SLOT_BASED_FSM_H
#define __SLOT_BASED_FSM_H

#include <stdint.h>
#include "StackBasedFsm.h"
#include "config.h"

#define MENU_MAX_SLOTS 12

typedef enum {
  INVALID_SLOT,
  TRANSIT_SLOT,
  HEXVALUE_SLOT
} SlotType_t;

typedef struct {
  char pLabel[MAX_MENU_ITEM_CHARS];
  TransitionFunc pNextStateTransitionFunc;
} TransitSlot_t;

typedef struct {
  char pLabel[MAX_MENU_ITEM_CHARS];
  bool inEditMode;
  int digitPos;
  uint32_t* pValue;
} HexValueSlot_t;

typedef struct {
  SlotType_t type;
  TransitSlot_t transitSlot;
  HexValueSlot_t hexValueSlot;
} MenuSlot_t;

typedef struct {
  uint16_t xPos;
  uint16_t yPos;
  uint8_t numSlots;
  uint8_t cursorPos;
  StackBasedFsm_t* pFsm;
  MenuSlot_t pSlot[MENU_MAX_SLOTS];
} SlotBasedMenu_t;

// menu functions:
void menuInit(SlotBasedMenu_t* pSbm, StackBasedFsm_t* pFsm, int16_t xPos, int16_t yPos);
void menuTick(SlotBasedMenu_t* pSbm);
void menuAddTransitSlot(SlotBasedMenu_t* pSbm, const char* pLabel, TransitionFunc pFunc);
void menuAddHexValueSlot(SlotBasedMenu_t* pSbm, const char* pLabel, uint32_t* pValue);
void menuDraw(SlotBasedMenu_t* pSbm);
void menuMoveCursorUp(SlotBasedMenu_t* pSbm);
void menuMoveCursorDown(SlotBasedMenu_t* pSbm);
void menuAction(SlotBasedMenu_t* pMenu, void* pArgs);
void menuBack(SlotBasedMenu_t* pMenu);

#endif // __SLOT_BASED_FSM_H
