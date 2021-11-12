#ifndef STEPPER_H_
#define STEPPER_H_

// Pins on Arduino
#define STEP_TOP 3
#define DIR_TOP 6
#define STEP_BOT 2
#define DIR_BOT 5
#define ENABLE 8

// Spin directions
#define CW LOW
#define CCW HIGH

// Store how to interact with the hardware on the next timer interrupt
typedef struct {
    bool    changeDirection;  // Should the stepper change directions
    uint8_t directionV;       // New direction to write to stepper
    bool    step;             // Should stepper step
    uint8_t stepV;            // Value to write to the stepper step
} StepperCommand;

// Take a stepper command and write to the hardware pins
void handle_stepper(StepperCommand *sc, char directionPin, char stepPin)

#endif //STEPPER_H_