#ifndef __EXAMPLESTATE_H__
#define __EXAMPLESTATE_H__

#include "../StackBasedFsm.h"

typedef struct {
  int someInt;
  int* pReturnValue;
} ExampleParams;

void mainMenu(StackBasedFsm_t* pFsm, FsmState* pState);

#endif // __EXAMPLESTATE_H__