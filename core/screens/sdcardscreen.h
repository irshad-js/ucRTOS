#ifndef __SDCARDSCREEN_H__
#define __SDCARDSCREEN_H__

typedef struct {
  int dummy;
} SdCardScreenParams;

void sdCardScreen(StackBasedFsm_t* pFsm, FsmState* pState);

#endif // __SDCARDSCREEN_H__

