// NOTE: CNC Sheild X axis cooresponds to the top stepper, and Y axis to bottom stepper.
#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "Globals.h"

// Pins on Arduino
#define STEP_TOP 3
#define DIR_TOP 6
#define STEP_BOT 2
#define DIR_BOT 5
#define ENABLE 8
// Spin directions
#define CW LOW
#define CCW HIGH

class Director
{
public:
    Director();
    void move(Direction, uint16_t);
    void enable();
    void disable();
    bool areEnabled();
    void resetHome();
    void setDelay(uint16_t);
    void printCoordinates();
    void travel(const Point &);
    void localTravel(int16_t x, int16_t y);
    void square(uint16_t);
    void diamond(uint16_t);

private:
    Point position;
    uint32_t delay;
    bool enabled;
};

#endif // DIRECTOR_H_