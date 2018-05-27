#ifndef __LOCK_FREE_FIFO_H
#define __LOCK_FREE_FIFO_H

#include <stdbool.h>
#include "../../core/config.h"

typedef struct {
  char pRingBuffer[RING_BUFFER_SIZE];
  int wptr;
  int rptr;
} LockFreeFIFO_t;

bool ringBufferDataAvailable(LockFreeFIFO_t* pLff);
void writeToRingBuffer(LockFreeFIFO_t* pLff, char b);
char readFromRingBuffer(LockFreeFIFO_t* pLff);

#endif // __LOCK_FREE_FIFO_H
