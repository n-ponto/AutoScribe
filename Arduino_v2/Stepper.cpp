#include "Stepper.h"

void handle_stepper(StepperCommand *sc, char directionPin, char stepPin)
{
    if (sc->changeDirection)
        digitalWrite(directionPin, sc->directionV);
    if (sc->step)
        digitalWrite(stepPin, sc->stepV);
} 