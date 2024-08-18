/*
Contains all code specific to the Accepting Commands runtime mode.

- accepting commands function to read commands from serial
- list of commands (functions to run for each commands)
*/

#include <Arduino.h>
#include <NOKIA5110_TEXT.h>

#include "Commands.h"

extern NOKIA5110_TEXT lcdScreen;

void acceptingCommands() {
  // lcdScreen.LCDClear(0x00);
  // lcdScreen.LCDgotoXY(0, 0);
  // lcdScreen.LCDString(" AutoScribe ");
  // Read a byte (command) from serial
  // Call the function corresponding to that commmand
  // Function will handle reading more data from serial
  // Have functions for switching commands (change the runtime mode for the main loop)
  unsigned char command = 0xFF;  // Read byte from serial
  while (true) {
    while (!Serial.available()) {
    }
    Serial.readBytes((char *)&command, 1);
    Serial.print("Received command 0x");
    Serial.println(command, HEX);

    switch (command) {
      case 1:
        setPenRange();
        break;
      case 2:
        changePenAngle();
        break;
      case 3:
        setStepperDelay();
        break;
      case 8:
        enterDrawMode();
        break;
      case 9:
        enterManualControlMode();
        break;
      default:
        Serial.println("Invalid command.");
        break;
    }

    if (command & RUNTIME_CHANGE)
      break;
  }
}