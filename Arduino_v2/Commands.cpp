/*
Contains the implementations for all the commands that can be called from the
Accepting Commands mode

All commands should be able to run in a deterministic amount of computations. They
should be simple and have no loops or nested functions.
*/

#include <Arduino.h>
#include <Servo.h>
#include <TimerTwo.h>

#include "Hardware.h"
#include "RuntimeModes.h"

extern Servo penServo;
extern uint8_t penUpAngle, penDownAngle;
extern uint16_t stepperPeriodDrawing;
extern uint8_t mstepMulti;
extern void (*changeDrawStepFn)();

void setPenRange() {
  // Read two bytes, one for the minimum then maximum angle for the pen's
  // movement
  uint8_t angles[2];
  while (Serial.available() < sizeof(angles)) {
  }
  Serial.readBytes((char *)&angles, sizeof(angles));
  penUpAngle = angles[0];
  penDownAngle = angles[1];
  Serial.println("Pen range modified.");
  return;
}

void changePenAngle() {
  // Read one byte and change the pen angle to that value
  // Will continue until it receives a value greater than 180
  Serial.println("Changing angle of the pen.");
  uint8_t angle = 0;
  while (true) {
    while (Serial.available() < 1) {
    }
    Serial.readBytes((char *)&angle, 1);
    if (angle > 180)
      break;
    penServo.write(angle);
    Serial.print("Angle set to ");
    Serial.println(angle);
  }
  return;
}

void enterDrawMode() {
  // Set the runtime mode to drawing to be called from the main loop
  Serial.println("Switching runtime mode to drawing.");
  setRuntimeMode(drawing);
}

void enterManualControlMode() {
  // Set runtime mode to Manual Control to be called from the main loop
  Serial.println("Switching runtime mode to manual control.");
  setRuntimeMode(manualControl);
}

void setStepperDelay() {
  // Read two bytes for the time to delay pulses to the stepper
  // The time is measured in microseconds and should be in the range 500-1200ish
  uint16_t speed;
  Serial.println("Waiting for stepper delay...");
  while (Serial.available() < 2) {
  }
  Serial.readBytes((char *)&speed, 2);

  speed *= SPEED_MULTIPLIER;  // Convert from mm/s to steps/s
  mstepMulti = 1;             // Reset the microstepping multiplier

  // Double multiplier until the speed is greater than the minimum
  while (speed * mstepMulti < MIN_STEPS_PER_SEC && mstepMulti <= MAX_MICROSTEP) {
    mstepMulti *= 2;
  }

  // Check if the multiplier is too high
  if (mstepMulti > MAX_MICROSTEP) {
    Serial.print("Cannot support speed of ");
    Serial.print(speed);
    Serial.println(" steps/s");
    return;
  }

  stepperPeriodDrawing = 1000000 / (speed * mstepMulti);  // Convert from steps/s to us/micro-step

  Serial.print("Microstepping multiplier set to ");
  Serial.println(mstepMulti);
  Serial.print("Stepper delay reset to ");
  Serial.print(stepperPeriodDrawing);
  Serial.println("us");

  // Update the function for microstepping
  changeDrawStepFn = getChangeStepFunction();
}
