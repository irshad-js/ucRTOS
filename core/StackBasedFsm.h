#ifndef __STACK_BASED_FSM_H
#define __STACK_BASED_FSM_H

#include <stdbool.h>
#include "config.h"

typedef struct FsmState FsmState;
typedef struct StackBasedFsm_t StackBasedFsm_t;

// Callback function pointer typedefs
typedef void(*OnButtonCallback)(StackBasedFsm_t* pFsm, bool pressed);
typedef void(*OnDirectionCallback)(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east);
typedef void(*OnEnterStateCallback)(StackBasedFsm_t* pFsm, void* pParams);
typedef void(*OnReenterStateCallback)(StackBasedFsm_t* pFsm);
typedef void(*OnLeaveStateCallback)(StackBasedFsm_t* pFsm);
typedef void(*OnTickCallback)(StackBasedFsm_t* pFsm);

struct FsmState {
  OnButtonCallback       onAction;
  OnButtonCallback       onBack;
  OnButtonCallback       onStart;
  OnButtonCallback       onSelect;
  OnDirectionCallback    onDirection;
  OnEnterStateCallback   onEnterState;
  OnReenterStateCallback onReenterState;
  OnLeaveStateCallback   onLeaveState;
  OnTickCallback         onTick;
};

struct StackBasedFsm_t {
  FsmState stack[FSM_STACK_SIZE];
  int stackSize_;
};

typedef void(*TransitionFunc)(StackBasedFsm_t* pFsm, FsmState* pState);

void fsmInit(StackBasedFsm_t* pFsm);
bool fsmPush(StackBasedFsm_t* pFsm, TransitionFunc pFunc, void* pArgs);
bool fsmPop(StackBasedFsm_t* pFsm);
FsmState* fsmGetCurrentState(StackBasedFsm_t* pFsm);
void fsmTick(StackBasedFsm_t* pFsm);

bool leaveState(StackBasedFsm_t* pFsm);

#endif

