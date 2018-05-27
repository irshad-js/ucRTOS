#ifndef __LIVEMODESCREEN_H__
#define __LIVEMODESCREEN_H__

typedef struct {
  int someInt;
  int* pReturnValue;
} LiveModeScreenParams;

void liveModeScreen(StackBasedFsm_t* pFsm, FsmState* pState);

#endif // __LIVEMODESCREEN_H__
