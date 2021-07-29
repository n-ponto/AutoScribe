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

class StepperController
{
public:
    StepperController();
    void move(Direction, uint16_t steps);
    void enable();
    void disable();
    bool areEnabled();
};

#endif // STEPPER_CONTROLLER_H_