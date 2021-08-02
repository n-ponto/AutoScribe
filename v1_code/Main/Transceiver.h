#ifndef TRANSCEIVER_H_
#define TRANSCEIVER_H_

#include "StepperController.h"

enum Command {
    MOVE_TO_COORDINATE,
    CHANGE_PEN_ANGLE,
    RESET_HOME
};

typedef struct {
    Command command;
    uint16_t param1;
    uint16_t param2;
} Message;

class Transceiver
{
public:
    void initialize(StepperController*, Servo*);
    void handle();
private:
    Message message;
    StepperController *stepper;
    Servo *servo;
    void moveToCoordinate(uint16_t x, uint16_t y);
    void changePenAngle(uint16_t angle);
    void resetHome();
};

#endif // TRANSCEIVER_H_