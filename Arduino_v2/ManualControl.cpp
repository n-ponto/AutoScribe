/*
Contains all code specific to the Manual Control runtime mode.
*/
#include <NOKIA5110_TEXT.h>
#include <Servo.h>
#include <TimerTwo.h>

#include "Hardware.h"
#include "RuntimeModes.h"

// Possible bytes passed from serial
#define STOP 0
#define SP_P 1
#define SP_R 2
#define UP_P 3
#define UP_R 4
#define DN_P 5
#define DN_R 6
#define LT_P 7
#define LT_R 8
#define RT_P 9
#define RT_R 10

extern Servo penServo;
extern uint8_t penUpAngle, penDownAngle;
extern NOKIA5110_TEXT lcdScreen;
extern bool penUp;
extern uint8_t mstepMulti;

volatile struct
{
  bool movePen, penUp;
  bool stepx, stepy;
  char writeVal;        // Only set by interrupt
  int16_t x, y;         // Global X and Y coordinates in terms of 16th steps
  int8_t xchng, ychng;  // +/- 1 based on how to change the coordinates each step
} mcState;

void manualControlInterrupt() {
  // Move steppers
  if (mcState.stepx) {
    digitalWrite(TOP_STP_PIN, mcState.writeVal);
    if (mcState.writeVal)
      mcState.x += penUp ? mcState.xchng << 4 : mcState.xchng * (16 / mstepMulti);
  }
  if (mcState.stepy) {
    digitalWrite(BOT_STP_PIN, mcState.writeVal);
    if (mcState.writeVal)
      mcState.y += penUp ? mcState.ychng << 4 : mcState.ychng * (16 / mstepMulti);
  }
  mcState.writeVal = !mcState.writeVal;
}

void startManualControl() {
  mcState.movePen = false;
  mcState.penUp = true;
  mcState.stepx = mcState.stepy = false;
  mcState.writeVal = HIGH;
  mcState.x = mcState.y = 0;
  Serial.println("Starting manual control mode.");
  digitalWrite(ENABLE_PIN, ENABLE_STEPPERS);  // Enable steppers
  digitalWrite(HALF_STEP_PIN, LOW);           // Enable full-steps
  penServo.write(penUpAngle);                 // Move the pen up
  Timer2.attachInterrupt(manualControlInterrupt);
  Timer2.start();

  // Set up the screen
  lcdScreen.LCDClear(0x00);                    // Clear the screen
  lcdScreen.LCDgotoXY(0, 0);                   // Go to the top left corner
  lcdScreen.LCDString("   MANUAL   ");  // Print the title
  lcdScreen.LCDgotoXY(0, 1);                   // Go to the top left corner
  lcdScreen.LCDString("   CONTROL  ");  // Print the title
  lcdScreen.LCDgotoXY(0, 4);                   // Go to the coordinate line
  lcdScreen.LCDString("X:");
  lcdScreen.LCDgotoXY(0, 5);  // Go to the next line
  lcdScreen.LCDString("Y:");
}

void endManualControl() {
  Timer2.stop();                               // Stop the timer
  Timer2.detachInterrupt();                    // Remove the interrupt
  digitalWrite(ENABLE_PIN, DISABLE_STEPPERS);  // Disable steppers
  setRuntimeMode(acceptingCommands);           // Return to Accepting Commands
  Serial.println("Manual control mode ended.");
}

void manualControl() {
  /*
  Read a byte from serial corresponding to a direction (or moving pen up/down)
  Convert the direction into a specific command (how to move which motors)
  If the queue is full then stop reading

  When receives the stop signal, immediately stop the timer interrupt
  Then change the mode back to accepting commands
  */
  char key;
  char xStr[8], yStr[8];
  unsigned long lastScreenUpdate = millis();
  startManualControl();
  while (1) {
    // Wait for serial
    while (Serial.available() < 1) {
    }

    Serial.readBytes((char *)&key, 1);
    if (key == STOP)
      break;

    switch (key) {
      case (SP_P):
        penDraw();
        break;
      case (SP_R):
        penMove();
        break;
      case (UP_P):
        digitalWrite(BOT_DIR_PIN, CW);
        mcState.stepy = true;
        mcState.ychng = -1;
        break;
      case (UP_R):
        mcState.stepy = false;
        break;
      case (DN_P):
        digitalWrite(BOT_DIR_PIN, CCW);
        mcState.stepy = true;
        mcState.ychng = +1;
        break;
      case (DN_R):
        mcState.stepy = false;
        break;
      case (LT_P):
        digitalWrite(TOP_DIR_PIN, CCW);
        mcState.stepx = true;
        mcState.xchng = -1;
        break;
      case (LT_R):
        mcState.stepx = false;
        break;
      case (RT_P):
        digitalWrite(TOP_DIR_PIN, CW);
        mcState.stepx = true;
        mcState.xchng = +1;
        break;
      case (RT_R):
        mcState.stepx = false;
        break;
    }

    // Update screen
    if (millis() - lastScreenUpdate > SCREEN_UPDATE_PERIOD) {
      sprintf(xStr, "%5d", mcState.x >> 4);    // Convert x to string
      sprintf(yStr, "%5d", mcState.y >> 4);    // Convert y to string
      lcdScreen.LCDgotoXY(SCRN_CH_WD * 2, 4);  // Jump over 'X: '
      lcdScreen.LCDString(xStr);
      lcdScreen.LCDgotoXY(SCRN_CH_WD * 2, 5);
      lcdScreen.LCDString(yStr);
      lastScreenUpdate = millis();
    }
  }
  endManualControl();
}