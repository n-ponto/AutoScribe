/*
Main file for the Arduino code
Contains:
- startup and loop functions
- shared buffer for the drawing and manual control modes
- list of functions for the runtime modes
*/
#include <Arduino.h>
#include <NOKIA5110_TEXT.h>
#include <Servo.h>
#include <TimerTwo.h>

#include "Commands.h"
#include "Hardware.h"
#include "RuntimeModes.h"

void (*runtime_mode)(void);  // Function to call the current runtime mode

Servo penServo;
NOKIA5110_TEXT lcdScreen(SCREEN_RST, SCREEN_CE, SCREEN_DC, SCREEN_DIN, SCREEN_CLK);

uint8_t penUpAngle, penDownAngle;  // Angle for the pen servo to be up and down
uint16_t stepperPeriodDrawing;     // Microsecond delay between stepper pulses when drawing
uint16_t stepperPeriodMoving;      // Microsecond delay between stepper pulses when moving
uint8_t mstepMulti;                // Multiplier for microstepping (1, 2, 4, 8, or 16)
bool penUp;                        // True if the pen is up, false if the pen is down

void (*changeDrawStepFn)();  // Function to change the microstepping mode for drawing

/// @brief Initialize pins and set default values for hardware to avoid undefined behavior
void hardwareInit() {
  Serial.begin(9600);
  // Initilize the stepper pins
  pinMode(TOP_STP_PIN, OUTPUT);
  pinMode(TOP_DIR_PIN, OUTPUT);
  pinMode(BOT_STP_PIN, OUTPUT);
  pinMode(BOT_DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(HALF_STEP_PIN, OUTPUT);
  pinMode(QRTR_STEP_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, DISABLE_STEPPERS);
  // initialize the pen servo pin
  penUpAngle = DEFAULT_UP;
  penDownAngle = DEFAULT_DOWN;
  penServo.attach(SERVO_PIN);
  penServo.write(20);
  // initialize stepper motors
  stepperPeriodMoving = DEFAULT_STEPPER_DELAY;
  updateStepperSpeed(DEFAULT_DRAWING_SPEED);
  // timer for interrupts and runtimeMode
  Timer2.init(stepperPeriodDrawing, NULL);
  runtime_mode = acceptingCommands;
}

void setup() {
  hardwareInit();

  waitForConnection();
}

void loop() {
  // Call the function for the current runtime mode
  runtime_mode();
}

void setRuntimeMode(void (*fn)(void)) {
  runtime_mode = fn;
}