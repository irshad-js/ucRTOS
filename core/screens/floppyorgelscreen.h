#ifndef __FLOPPYORGELSCREEN_H__
#define __FLOPPYORGELSCREEN_H__

typedef struct {
  int dummy; // TODO: add params if needed  
} FloppyOrgelScreenParams;

void floppyOrgelScreen(StackBasedFsm_t* pFsm, FsmState* pState);

#endif // __FLOPPYORGELSCREEN_H__
