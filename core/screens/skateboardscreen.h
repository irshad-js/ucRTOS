#ifndef __SKATEBOARDSCREEN_H__
#define __SKATEBOARDSCREEN_H__

typedef struct {
  int someInt;
  int* pReturnValue;
} SkateboardScreenParams;

void skateboardScreen(StackBasedFsm_t* pFsm, FsmState* pState);

#endif // __SKATEBOARDSCREEN_H__

