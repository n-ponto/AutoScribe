#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <Nokia_LCD.h>

// extern Nokia_LCD lcd;

void displayInit();

void displayManualControl();
void displayDrawing();

void updateCoordinateDisplay(int16_t x, int16_t y);
void updateInstructionCountDisplay(int total, int count);


#endif // DISPLAY_H_