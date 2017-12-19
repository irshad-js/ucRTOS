#ifndef __MAINMENUSCREEN_H__
#define __MAINMENUSCREEN_H__

#include "../StackBasedFsm.h"

typedef struct {
  int someInt;
  int* pReturnValue;
} MainMenuScreenParams;

void mainMenuScreen(StackBasedFsm_t* pFsm, FsmState* pState);

#endif // __MAINMENUSCREEN_H__
