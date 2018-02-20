#ifndef __LUT_H__
#define __LUT_H__

#include <stdint.h>

typedef struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} Color;

extern Color pMasterPalette[16];
extern uint8_t pClut565[65535];

#endif // !__LUT_H__
