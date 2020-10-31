#ifndef __MAGICLANTERNSCREEN_H__
#define __MAGICLANTERNSCREEN_H__

typedef struct {
  int someInt;
  int* pReturnValue;
} MagicLanternScreenParams;

void magicLanternScreen(StackBasedFsm_t* pFsm, FsmState* pState);

#endif // __EXAMPLESCREEN_H__
