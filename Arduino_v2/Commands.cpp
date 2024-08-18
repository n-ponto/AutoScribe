/*
Contains the implementations for all the commands that can be called from the
Accepting Commands mode

All commands should be able to run in a deterministic amount of computations. They
should be simple and have no loops or nested functions.
*/

#include <Arduino.h>
#include <NOKIA5110_TEXT.h>
#include <Servo.h>
#include <TimerTwo.h>

#include "Hardware.h"
#include "RuntimeModes.h"

extern Servo penServo;
extern NOKIA5110_TEXT lcdScreen;
extern uint8_t penUpAngle, penDownAngle;
extern uint16_t stepperPeriodDrawing;
extern uint8_t mstepMulti;
extern void (*changeDrawStepFn)();

/// @brief Read bytes from serial and set the range of angles for the pen servo
void setPenRange() {
  uint8_t angles[2];
  while (Serial.available() < sizeof(angles)) {
  }
  Serial.readBytes((char *)&angles, sizeof(angles));
  penUpAngle = angles[0];
  penDownAngle = angles[1];
  Serial.println("Pen range modified.");
}

void changePenAngle() {
  // Read one byte and change the pen angle to that value
  // Will continue until it receives a value greater than 180
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
}

void enterDrawMode() {
  // Set the runtime mode to drawing to be called from the main loop
  setRuntimeMode(drawing);
}

void enterManualControlMode() {
  // Set runtime mode to Manual Control to be called from the main loop
  setRuntimeMode(manualControl);
}

void setStepperDelay() {
  // Read two bytes for the time to delay pulses to the stepper
  // The time is measured in microseconds and should be in the range 500-1200ish
  uint16_t speed;
  while (Serial.available() < 2) {
  }
  Serial.readBytes((char *)&speed, 2);
  updateStepperSpeed(speed);
}

/// @brief Perform the handshake with the computer to initialize
void waitForConnection() {
  delay(50);
  lcdScreen.LCDInit(false, SCREEN_CONTRAST, SCREEN_BIAS);  // init  the lCD
  lcdScreen.LCDClear(0x00);                                // Clear whole screen
  lcdScreen.LCDFont(LCDFont_Default);                      // Set the font
  lcdScreen.LCDgotoXY(0, 0);                               // (go to (X , Y) (0-84 columns, 0-5 blocks) top left corner
  lcdScreen.LCDString(" AutoScribe ");                     // Print the title
  delay(50);

  lcdScreen.LCDgotoXY(0, 2);
  lcdScreen.LCDString("Start.......");
  lcdScreen.LCDgotoXY(0, 3);
  lcdScreen.LCDString("Connect.....");
  lcdScreen.LCDgotoXY(0, 4);
  lcdScreen.LCDString("Data........");

  while (!Serial) {
    delay(50);
  }

  lcdScreen.LCDgotoXY(70, 2);
  lcdScreen.LCDString("OK");

  // Wait for handshake response from computer
  Serial.write(HANDSHAKE);
  while (Serial.read() != HANDSHAKE) {
    delay(50);
    Serial.write(HANDSHAKE);
  }

  lcdScreen.LCDgotoXY(70, 3);
  lcdScreen.LCDString("OK");

  // Read pen heights
  setPenRange();

  // Read stepper speed
  setStepperDelay();

  lcdScreen.LCDgotoXY(70, 4);
  lcdScreen.LCDString("OK");
}
