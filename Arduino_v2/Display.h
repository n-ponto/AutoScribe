#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <Nokia_LCD.h>

// extern Nokia_LCD lcd;

void displayInit();

void displayManualControl();

void updateCoordinateDisplay(uint16_t x, uint16_t y);

#endif // DISPLAY_H_