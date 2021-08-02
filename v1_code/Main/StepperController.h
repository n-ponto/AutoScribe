// NOTE: CNC Sheild X axis cooresponds to the top stepper, and Y axis to bottom stepper.
#ifndef STEPPER_CONTROLLER_H_
#define STEPPER_CONTROLLER_H_

// Pins on Arduino
#define STEP_TOP 3
#define DIR_TOP  6
#define STEP_BOT 2
#define DIR_BOT  5
#define ENABLE   8
// Spin directions
#define CW       LOW
#define CCW      HIGH

typedef uint8_t Direction;
#define UP    0b1000
#define DOWN  0b0100
#define LEFT  0b0010
#define RIGHT 0b0001

typedef struct {
    uint16_t x;
    uint16_t y;
} Point;

class StepperController
{
public:
    void initialize();
    void move(Direction, uint16_t);
    void enable();
    void disable();
    bool areEnabled();
    void square(uint16_t);
    void diamond(uint16_t);
    void resetHome();
    void printCoordinates();
    void travel(const Point &);
    void localTravel(int16_t x, int16_t y);
    Point position;
private:
    uint32_t wait;
    bool enabled;
};

#endif // STEPPER_CONTROLLER_H_