#ifndef __EXAMPLESCREEN_H__
#define __EXAMPLESCREEN_H__

typedef struct {
  int someInt;
  int* pReturnValue;
} ExampleParams;

void exampleScreen(StackBasedFsm_t* pFsm, FsmState* pState);

#endif // __EXAMPLESCREEN_H__