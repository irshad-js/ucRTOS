#include "display.h"

void displayInit() {
  hardwareDisplayInit();
}

void displaySetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) {
  hardwareDisplaySetPixel(x, y, red, green, blue);
}

void displayDraw() {
  hardwareDisplayDraw();
}

void displayClear() {
  hardwareDisplayClear();
}

void displayDrawImage(uint16_t xPos, uint16_t yPos, const uint8_t* pImg) {
	uint8_t width = pImg[0];
	uint8_t height = pImg[1];
	pImg += 2;

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {
			uint8_t red = pImg[3 * (x + y * width) + 0];
			uint8_t green = pImg[3 * (x + y * width) + 1];
			uint8_t blue = pImg[3 * (x + y * width) + 2];

			if (!(red == 255 && green == 0 && blue == 255)) // Is the pixel transparent?
				displaySetPixel(xPos + x, yPos + y, red, green, blue);
		}
}
