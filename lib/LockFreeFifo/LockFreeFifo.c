#include "../../lib/colorprint/colorprint.h" // TODO: remove after debug!
#include "LockFreeFifo.h"

static int getRingBufferDistance(LockFreeFIFO_t* pLff) {
  if (pLff->rptr > pLff->wptr)
    return pLff->rptr - pLff->wptr;
  else
    return pLff->rptr - pLff->wptr + RING_BUFFER_SIZE;  
}

bool ringBufferDataAvailable(LockFreeFIFO_t* pLff) {
  return getRingBufferDistance(pLff) < RING_BUFFER_SIZE;
}

void writeToRingBuffer(LockFreeFIFO_t* pLff, char b) {
  if (getRingBufferDistance(pLff) == 1) {
    hal_printfWarning("Ring buffer overflow!\n\r"); // TODO: remove after debug!
    return;
  }

  pLff->pRingBuffer[pLff->wptr] = b;
  pLff->wptr = (pLff->wptr + 1) % RING_BUFFER_SIZE;
}

char readFromRingBuffer(LockFreeFIFO_t* pLff) {
  if (getRingBufferDistance(pLff) == RING_BUFFER_SIZE) {
    hal_printfWarning("Ring buffer underflow!\n\r"); // TODO: remove after debug!
    return 0;
  }

  char ret = pLff->pRingBuffer[pLff->rptr];
  pLff->rptr = (pLff->rptr + 1) % RING_BUFFER_SIZE;
  return ret;
}
