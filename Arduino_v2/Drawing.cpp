/*
Contains all code specific to the Drawing runtime mode.
*/

#ifndef TESTING
#include <Arduino.h>
#include <TimerTwo.h>
#include <Servo.h>
#include "RuntimeModes.h"
#include "Display.h"
#else
extern int drawLoopCounter;
#include "../CTests/DrawingMocks.h"
#endif //TESTING
#include "Queue.h"
#include "Hardware.h"
#include "Drawing.h"

// #define VERBOSE_DEBUG

extern Servo penServo;
extern uint8_t penUpAngle, penDownAngle;
extern uint16_t stepperDelay;
uint16_t penDelaySteps; // Interrupt steps to pause while pen is moving
bool penUp;

// Current state of the line drawing algorithm
// between calls to the drawing interrupt
struct ds
{
    Point pt;
    int16_t i;
    int16_t absx;
    int16_t slopeError;
    int16_t c, m;
    bool swapxy;
} drawState;

// Next instruction to write to the stepper motors
// at the beginning of the next interrupt
struct ns
{
    uint8_t stepPin, writeVal;   // Pin 'stepPin' to write 'writeVal' to
    bool changeXDir, changeYDir; // Should change x or y motors' direction
    uint8_t newXDir, newYDir;    // New direction for each motor
} nextStep;

unsigned char buffer[BUFSZ];   // Data area for the queue
Queue queue;                   // FIFO queue for the coordinates received
volatile bool continueDrawing; // Set by interrupt to leave drawing mode when complete

void printPoint(Point *pt)
{
    Serial.print("Received\tX 0x");
    Serial.print(pt->x, HEX);
    Serial.print("\tY 0x");
    Serial.println(pt->y, HEX);
}

// Set up the drawing state with new dequeued point
void initDrawState(Point *pt)
{
    bool back = pt->x & NEG_BIT;
    bool down = pt->y < 0;
    int16_t absy;
    drawState.absx = back ? -1 * pt->x : pt->x;
    absy = down ? -1 * pt->y : pt->y;
    drawState.swapxy = absy > drawState.absx;

    // Check if need to switch x and y
    if (drawState.swapxy)
    {
        int16_t save = drawState.absx;
        drawState.absx = absy;
        absy = save;
    }

    drawState.m = 2 * absy;
    drawState.slopeError = drawState.m - drawState.absx;
    drawState.c = drawState.slopeError - drawState.absx;
    drawState.i = 0;
    drawState.pt = *pt;
}

// Timer interrupt function
void drawingInterrupt()
{
    if (penDelaySteps > 0)
    {
        penDelaySteps--;
        return;
    }
    /*************** MOVE HARDWARE ***************/
    // Change stepper directions
    if (nextStep.changeXDir)
    {
        digitalWrite(TOP_DIR_PIN, nextStep.newXDir);
        nextStep.changeXDir = false;
    }
    if (nextStep.changeYDir)
    {
        digitalWrite(BOT_DIR_PIN, nextStep.newYDir);
        nextStep.changeYDir = false;
    }

    // Step the motors
    if (nextStep.stepPin == STEP_BOTH)
    {
        digitalWrite(TOP_STP_PIN, nextStep.writeVal);
        digitalWrite(BOT_STP_PIN, nextStep.writeVal);
    }
    else if (nextStep.stepPin)
    {
        digitalWrite(nextStep.stepPin, nextStep.writeVal);
    }

    // If wrote high to either stepper then write low next and return
    if (nextStep.writeVal)
    {
        // Write to the same stepPin
        nextStep.writeVal = LOW;
        return;
    }

    /*************** CHECK IF REACHED COORDINATE ***************/
    if (drawState.i >= drawState.absx)
    {
        // Dequeue a new point
        Point newPt = {0, 0};
        if (isEmpty(&queue))
        { // If the queue is empty
#ifdef VERBOSE_DEBUG
            printf("Buffer empty\n");
#endif
            return; // Check again during the next interrupt
        }
        dequeue(&queue, &newPt);

        // Check for stop signal
        if (newPt.x & STOP_DRAWING)
        {
#ifdef VERBOSE_DEBUG
            printf("Dequeued stop drawing point\n");
#endif
            continueDrawing = false;
            return;
        }

        // Handle pen
        if (newPt.x & MOVE_PEN) // If point not connected to previous
        {
            penServo.write(penUpAngle); // Raise the pen
            penUp = true;
            penDelaySteps = DEFAULT_PEN_DELAY / (stepperDelay / 1000);
        }
        else if (penUp)
        {
            penServo.write(penDownAngle); // Lower the pen
            penUp = false;
            penDelaySteps = DEFAULT_PEN_DELAY / (stepperDelay / 1000);
        }
#ifdef TESTING
        penDelaySteps = 0; // Don't bother delaying if we're testing
#endif
        // TODO: add delay to this function to wait for the pen to raise/lower

        newPt.x = CONVERT_ETS(newPt.x); // Convert from eleven to sixteen bit number

#ifdef VERBOSE_DEBUG
        printf("Dequeue point: (%hd, %hd)\n", newPt.x, newPt.y);
#endif

        // Compare x's, if different sign then change stepper direction for new point
        if (DIFF_SIGN(drawState.pt.x, newPt.x))
        {
            nextStep.changeXDir = true;
            nextStep.newXDir = newPt.x & NEG_BIT ? CCW : CW;
        }
        // Compare y's, if different sign then change stepper direction for new point
        if (DIFF_SIGN(drawState.pt.y, newPt.y))
        {
            nextStep.changeYDir = true;
            nextStep.newYDir = newPt.y & NEG_BIT ? CW : CCW;
        }
        // Initialize the drawing state for the next coordinate
        initDrawState(&newPt);
    }

    /*************** CONTINUE LINE ***************/
    // Continue the line drawing algorithm and queue the next instruction
    nextStep.writeVal = HIGH;
    if (drawState.slopeError >= 0)
    {
        nextStep.stepPin = STEP_BOTH;
        drawState.slopeError += drawState.c;
    }
    else
    {
        nextStep.stepPin = drawState.swapxy ? BOT_STP_PIN : TOP_STP_PIN;
        drawState.slopeError += drawState.m;
    }
    drawState.i++;
}

// Set up variables when drawing mode first starts
void startDrawing()
{
    Serial.println("Starting drawing mode.");
    queueInit(&queue, buffer, sizeof(Point), BUFSZ); // Initialize queue
    continueDrawing = true;                          // Loop drawing mode
    nextStep = {0, LOW, false, false, 0, 0};         // Clear next motor commands
    drawState = {0, 0, 0, 0, 0};                     // Clear drawing state
    digitalWrite(ENABLE_PIN, ENABLE_STEPPERS);       // Enable stepper motors
    digitalWrite(TOP_DIR_PIN, CW);                   // Initialize both steppers
    digitalWrite(BOT_DIR_PIN, CCW);
    penServo.write(penUpAngle); // Start the pen up
    penUp = true;
    Timer2.attachInterrupt(drawingInterrupt); // Set interrupt function
    Timer2.start();                           // Start the timer interrupt
    displayDrawing();                         // Show the drawing screen
}

// Clean up when drawing mode ends
void endDrawing()
{
    Timer2.stop();                              // Stop the timer interrupt
    Timer2.detachInterrupt();                   // Remove the interrupt function
    digitalWrite(ENABLE_PIN, DISABLE_STEPPERS); // Disable stepper motors
    penServo.write(penUpAngle);                 // End with the pen up
    setRuntimeMode(acceptingCommands);          // Return to Accepting Commands
    Serial.println("Ending drawing mode.");
}

void drawingLoop()
{
    Point previous, read, change;
    read = {0, 0};
    previous = {0, 0}; // Starts at the origin
    uint16_t flags;
    while (continueDrawing)
    {
        while (isFull(&queue))
        {
#ifdef VERBOSE_DEBUG
            printf("Buffer full\n");
            return;
#endif
            if (!continueDrawing)
                return;
            delay(10); // Wait 10ms before checking again
        }

        // Wait for a point on serial
        while (Serial.available() < POINTSZ)
            if (!continueDrawing)
                return;

        Serial.readBytes((char *)&read, POINTSZ);
        if (read.x == EMERGENCY_STOP)
            break;

        // Strip the flags and convert to 16 bit 2's compliment encoding
        flags = read.x & FLAG_MASK;
        read.x = CONVERT_ETS(read.x);

#ifdef VERBOSE_DEBUG
        printf("Received point: (0x%04hX, 0x%04hX) (%hi, %hi)\tFlags: 0x%04hX\n", read.x, read.y, read.x, read.y, flags);
#endif

        // Queue the change from previous coordinate
        change.x = (read.x - previous.x) & ~FLAG_MASK; // Convert back into eleven bit encoding
        change.y = read.y - previous.y;
        change.x |= flags; // add the flags back to the change
        enqueue(&queue, &change);
#ifdef VERBOSE_DEBUG
        printf("\tenqueue point: (0x%04hX, 0x%04hX) (%hi, %hi) %s %s\n", CONVERT_ETS(change.x), change.y, CONVERT_ETS(change.x), change.y, change.x & MOVE_PEN ? "\tMOVE" : "", change.x & STOP_DRAWING ? "\tSTOP" : "");
#endif

        previous = read; // Update the previous position
#ifdef TESTING
        if (--drawLoopCounter < 1)
            return;
#endif
    }
}

// The main thread for reading and queueing instructions from serial
void drawing()
{
    startDrawing();
    drawingLoop();
    endDrawing();
}