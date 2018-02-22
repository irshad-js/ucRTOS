#ifndef __PLAYERSCREEN_H__
#define __PLAYERSCREEN_H__

typedef struct {
  int someInt;
  int* pReturnValue;
} PlayerScreenParams;

void playerScreen(StackBasedFsm_t* pFsm, FsmState* pState);

#endif // __PLAYERSCREEN_H__
