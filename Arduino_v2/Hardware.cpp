#include "Hardware.h"

#include <Arduino.h>
#include <Servo.h>
#include <TimerTwo.h>

extern Servo penServo;
extern uint8_t penUpAngle, penDownAngle;
extern uint16_t stepperPeriodDrawing, stepperPeriodMoving;
extern uint8_t mstepMulti;
extern void (*changeDrawStepFn)();

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
}

void penMove() {
  penServo.write(penUpAngle);
  Timer2.setPeriod(stepperPeriodMoving);
  fullStep();
}
