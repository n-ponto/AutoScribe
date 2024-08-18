#include "Hardware.h"

#include <Arduino.h>
#include <Servo.h>
#include <TimerTwo.h>

extern Servo penServo;
extern uint8_t penUpAngle, penDownAngle;
extern uint16_t stepperPeriodDrawing, stepperPeriodMoving;
extern uint8_t mstepMulti;
extern void (*changeDrawStepFn)();
extern bool penUp;

void fullStep() {
  digitalWrite(HALF_STEP_PIN, LOW);
  digitalWrite(QRTR_STEP_PIN, LOW);
}

void halfStep() {
  digitalWrite(HALF_STEP_PIN, HIGH);
  digitalWrite(QRTR_STEP_PIN, LOW);
}

void quarterStep() {
  digitalWrite(HALF_STEP_PIN, LOW);
  digitalWrite(QRTR_STEP_PIN, HIGH);
}

void eighthStep() {
  digitalWrite(HALF_STEP_PIN, HIGH);
  digitalWrite(QRTR_STEP_PIN, HIGH);
}

void (*getChangeStepFunction(void))(void) {
  switch (mstepMulti) {
    case 1:
      // Should already be in this state from raisePen
      return fullStep;
    case 2:
      return halfStep;
    case 4:
      return quarterStep;
    case 8:
      return eighthStep;
    default:
      Serial.print("Invalid microstep multiplier ");
      Serial.println(mstepMulti);
      return NULL;
  }
}

void penDraw() {
  penServo.write(penDownAngle);
  Timer2.setPeriod(stepperPeriodDrawing);
  changeDrawStepFn();
  penUp = false;
}

void penMove() {
  penServo.write(penUpAngle);
  Timer2.setPeriod(stepperPeriodMoving);
  fullStep();
  penUp = true;
}

void updateStepperSpeed(uint16_t speed) {
  // Read two bytes for the time to delay pulses to the stepper
  // The time is measured in microseconds and should be in the range 500-1200ish
  speed *= SPEED_MULTIPLIER;  // Convert from mm/s to steps/s
  mstepMulti = 1;             // Reset the microstepping multiplier

  // Double multiplier until the speed is greater than the minimum
  while (speed * mstepMulti < MIN_STEPS_PER_SEC && mstepMulti <= MAX_MICROSTEP) {
    mstepMulti *= 2;
  }

  // Check if the multiplier is too high
  if (mstepMulti > MAX_MICROSTEP) {
    Serial.print("WARNING: Speed lower than recommended for microstepping ");
    Serial.print(speed);
    Serial.println(" steps/s");
    mstepMulti = MAX_MICROSTEP;
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
