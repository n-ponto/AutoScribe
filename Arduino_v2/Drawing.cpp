/*
Contains all code specific to the Drawing runtime mode.
*/
#ifndef TESTING
#include <Arduino.h>
#include <Servo.h>
#include <TimerTwo.h>

#include "RuntimeModes.h"
#else
extern int drawLoopCounter;
extern bool queueLowFlag;
#include "../CTests/DrawingMocks.h"
#endif  // TESTING
#include "Drawing.h"
#include "Hardware.h"
#include "Queue.h"

// #define VERBOSE_DEBUG

#define SIGNAL_START_SENDING 0xFA
#define SIGNAL_BUFFER_EMPTY 0xFF
#define SIGNAL_DONE_DRAWING 0xAB

extern uint16_t stepperPeriodDrawing, stepperPeriodMoving;
extern uint8_t mstepMulti;
uint16_t penDelaySteps;  // Interrupt steps to pause while pen is moving
bool penUp;
int executedCount = 0;

// Current state of the line drawing algorithm
// between calls to the drawing interrupt
struct ds {
  Point pt;
  int32_t i;
  int32_t absx;
  int32_t slopeError;
  int32_t c, m;
  bool swapxy;
} drawState;

// Next instruction to write to the stepper motors
// at the beginning of the next interrupt
struct ns {
  uint8_t stepPin, writeVal;    // Pin 'stepPin' to write 'writeVal' to
  bool changeXDir, changeYDir;  // Should change x or y motors' direction
  uint8_t newXDir, newYDir;     // New direction for each motor
} nextStep;

unsigned char buffer[Q_BUFSZ];  // Data area for the queue
Queue queue;                    // FIFO queue for the coordinates received
volatile bool continueDrawing;  // Set by interrupt to leave drawing mode when complete
Point prevCoordinate;           // Previous point reached

// Set up the drawing state with new dequeued point
void initDrawState(Point *pt, int multiplier) {
  bool back = pt->x & NEG_BIT;
  bool down = pt->y < 0;
  int16_t absy;
  drawState.absx = back ? -1 * pt->x : pt->x;
  absy = down ? -1 * pt->y : pt->y;
  drawState.swapxy = absy > drawState.absx;

  // Check if need to switch x and y
  if (drawState.swapxy) {
    int16_t save = drawState.absx;
    drawState.absx = absy;
    absy = save;
  }
  drawState.absx *= multiplier;
  absy *= multiplier;

  drawState.m = 2 * absy;
  drawState.slopeError = drawState.m - drawState.absx;
  drawState.c = drawState.slopeError - drawState.absx;
  drawState.i = 0;
  drawState.pt = *pt;
}

// Timer interrupt function
void drawingInterrupt() {
  if (drawState.i >= drawState.absx && isEmpty(&queue)) {  // Waiting for new point
#ifdef TESTING
    printf("BUFFER EMPTY\n");
    queueLowFlag = true;
#endif
    return;
  }
  if (penDelaySteps > 0) {
    penDelaySteps--;
    return;
  }
  /*************** MOVE HARDWARE ***************/
  // Change stepper directions
  if (nextStep.changeXDir) {
    digitalWrite(TOP_DIR_PIN, nextStep.newXDir);
    nextStep.changeXDir = false;
  }
  if (nextStep.changeYDir) {
    digitalWrite(BOT_DIR_PIN, nextStep.newYDir);
    nextStep.changeYDir = false;
  }

  // Step the motors
  if (nextStep.stepPin == STEP_BOTH) {
    digitalWrite(TOP_STP_PIN, nextStep.writeVal);
    digitalWrite(BOT_STP_PIN, nextStep.writeVal);
  } else if (nextStep.stepPin) {
    digitalWrite(nextStep.stepPin, nextStep.writeVal);
  }

  // If wrote high to either stepper then write low next and return
  if (nextStep.writeVal) {
    // Write to the same stepPin
    nextStep.writeVal = LOW;
    return;
  }

  /*************** CHECK IF REACHED COORDINATE ***************/
  if (drawState.i >= drawState.absx) {
    executedCount++;
    // Dequeue a new point
    Point newPt = {0, 0};
    if (isEmpty(&queue)) {  // If the queue is empty
      return;               // Check again during the next interrupt
    }
    dequeue(&queue, &newPt);

    // Check for stop signal
    if (newPt.x & STOP_DRAWING) {
#ifdef VERBOSE_DEBUG
      printf("Dequeued stop drawing point\n");
#endif
      continueDrawing = false;
      return;
    }

    // Handle pen
    int multiplier = mstepMulti;
    if (newPt.x & MOVE_PEN) {  // If point not connected to previous
      // penServo.write(penUpAngle);  // Raise the pen
      penMove();
      penUp = true;
      penDelaySteps = DEFAULT_PEN_DELAY / (stepperPeriodMoving / 1000);
      // Move at full speed
      multiplier = 1;
      // digitalWrite(HALF_STEP_PIN, LOW);
    } else if (penUp) {
      penDraw();
      // penServo.write(penDownAngle);  // Lower the pen
      penUp = false;
      penDelaySteps = DEFAULT_PEN_DELAY / (stepperPeriodDrawing / 1000);
      // Move 2x as many steps
      // digitalWrite(HALF_STEP_PIN, HIGH);
    }
#ifdef TESTING
    penDelaySteps = 0;  // Don't bother delaying if we're testing
#endif
    newPt.x = CONVERT_FTS(newPt.x);  // Convert from thirteen to sixteen bit number

#ifdef VERBOSE_DEBUG
    printf("Dequeue point: (%hd, %hd)\n", newPt.x, newPt.y);
#endif

    // Compare x's, if different sign then change stepper direction for new point
    if (DIFF_SIGN(drawState.pt.x, newPt.x)) {
      nextStep.changeXDir = true;
      nextStep.newXDir = newPt.x & NEG_BIT ? CCW : CW;
    }
    // Compare y's, if different sign then change stepper direction for new point
    if (DIFF_SIGN(drawState.pt.y, newPt.y)) {
      nextStep.changeYDir = true;
      nextStep.newYDir = newPt.y & NEG_BIT ? CW : CCW;
    }
    // Initialize the drawing state for the next coordinate
#ifdef TESTING
    multiplier = 1;
#endif
    initDrawState(&newPt, multiplier);
  }

  /*************** CONTINUE LINE ***************/
  // Continue the line drawing algorithm and queue the next instruction
  nextStep.writeVal = HIGH;
  if (drawState.slopeError >= 0) {
    nextStep.stepPin = STEP_BOTH;
    drawState.slopeError += drawState.c;
  } else {
    nextStep.stepPin = drawState.swapxy ? BOT_STP_PIN : TOP_STP_PIN;
    drawState.slopeError += drawState.m;
  }
  drawState.i++;
}

void startDrawing() {
  Serial.println("Starting drawing mode.");
  queueInit(&queue, buffer, sizeof(Point), Q_BUFSZ);  // Initialize queue
  continueDrawing = true;                             // Loop drawing mode
  nextStep = {0, LOW, false, false, 0, 0};            // Clear next motor commands
  drawState = {0, 0, 0, 0, 0};                        // Clear drawing state
  digitalWrite(ENABLE_PIN, ENABLE_STEPPERS);          // Enable stepper motors
  digitalWrite(TOP_DIR_PIN, CW);                      // Initialize both steppers
  digitalWrite(BOT_DIR_PIN, CCW);
  digitalWrite(HALF_STEP_PIN, HIGH);  // Half step
  prevCoordinate = {0, 0};            // Start at the origin
  penMove();                          // Start with the pen up
  penUp = true;
  Timer2.attachInterrupt(drawingInterrupt);  // Set interrupt function
  Timer2.start();                            // Start the timer interrupt
  Serial.println("Drawing started");
}

/// End the drawing mode
void endDrawing() {
  Timer2.stop();                               // Stop the timer interrupt
  Timer2.detachInterrupt();                    // Remove the interrupt function
  digitalWrite(ENABLE_PIN, DISABLE_STEPPERS);  // Disable stepper motors
  penMove();                                   // Move the pen up
  setRuntimeMode(acceptingCommands);           // Return to Accepting Commands
  Serial.write(SIGNAL_DONE_DRAWING);           // Send the end of drawing signal
  // Clear the serial buffer
  while (Serial.available() > 0) {
    Serial.read();
  }
  Serial.println("Drawing ended");
}

void drawingLoop() {
  Point pnt, change;
  pnt = {0, 0};
  uint16_t flags;
  int instructionCount = 0;
  uint8_t bufferPts = RX_BUFSZ / POINTSZ;  // Number of points that can fit in the RX buffer

  /* buf_space indicates how many points can fit into the RX buffer. The UNO
  has a RX buffer size of 64 bytes or 16 points. Assuming the computer starts
  by sending 64 bytes it's assumed there's no space in the buffer. As each
  point is read from the buffer, the buf_space increases by 1. */
  uint8_t buf_space = 0;
  // Wait for the signal to start accepting points
  while (Serial.read() != SIGNAL_START_SENDING) {
    delay(100);
  }
  Serial.write(SIGNAL_START_SENDING);  // Signal to start sending points
  while (continueDrawing) {
    if (isFull(&queue)) {
#ifdef TESTING
      printf("BUFFER FULL\n");
      return;
#endif
      delay(500);  // Wait 500ms before checking again
      continue;
    }

    /* Once the entire RX buffer has been read and there's space in the queue
    for another RX buffer worth of points, then send the signal to transmit
    more points and reset the space in the buffer. */
    if (buf_space >= bufferPts && Q_BUFSZ > queue.curSz + bufferPts) {
      buf_space = 0;
      Serial.write(SIGNAL_BUFFER_EMPTY);
    }

    // While waiting for another point retransmit the send signal.
    if (Serial.available() < POINTSZ)
      continue;

    Serial.readBytes((char *)&pnt, POINTSZ);
    buf_space++;
    if (pnt.x == EMERGENCY_STOP) {
      // Leave the execution loop when receives the emergency stop command
      return;
    }

    // Strip the flags and convert to 16 bit 2's compliment encoding
    flags = pnt.x & FLAG_MASK;
    pnt.x = CONVERT_FTS(pnt.x);

#ifdef VERBOSE_DEBUG
    printf("Received point: (0x%04hX, 0x%04hX) (%hi, %hi)\tFlags: 0x%04hX\n", pnt.x, pnt.y, pnt.x, pnt.y, flags);
#endif

    // Queue the change from previous coordinate
    change.x = (pnt.x - prevCoordinate.x) & ~FLAG_MASK;  // Convert back into eleven bit encoding
    change.y = pnt.y - prevCoordinate.y;
    change.x |= flags;  // add the flags back to the change
    enqueue(&queue, &change);
#ifdef VERBOSE_DEBUG
    printf("\tenqueue point: (0x%04hX, 0x%04hX) (%hi, %hi) %s %s\n", CONVERT_FTS(change.x), change.y, CONVERT_FTS(change.x), change.y, change.x & MOVE_PEN ? "\tMOVE" : "", change.x & STOP_DRAWING ? "\tSTOP" : "");
#endif

    prevCoordinate = pnt;  // Update the previous position
    instructionCount++;
#ifdef TESTING
    if (--drawLoopCounter < 1)
      return;
#endif
  }
}

// The main thread for reading and queueing instructions from serial
void drawing() {
  startDrawing();
  drawingLoop();
  endDrawing();
}