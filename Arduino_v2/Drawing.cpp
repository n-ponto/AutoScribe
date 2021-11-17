/*
Contains all code specific to the Drawing runtime mode.
*/

#ifndef TESTING
#include <TimerOne.h>
#include "RuntimeModes.h"
#else
extern int drawLoopCounter;
#include "../CTests/DrawingMocks.h"
#endif //TESTING
#include "Queue.h"
#include "Stepper.h"
#include "Drawing.h"

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
    bool back = pt->x < 0;
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
    /*************** STEP MOTORS ***************/
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
        Point newPt;
        dequeue(&queue, &newPt);
        // Check for stop signal
        if (newPt.x == STOP_DRAWING)
        {
            continueDrawing = false;
            return;
        }
        // Compare x's, if different sign then change stepper direction for new point
        if (DIFF_SIGN(drawState.pt.x, newPt.x))
        {
            nextStep.changeXDir = true;
            nextStep.newXDir = newPt.x >= 0 ? CW : CCW;
        }
        // Compare y's, if different sign then change stepper direction for new point
        if (DIFF_SIGN(drawState.pt.y, newPt.y))
        {
            nextStep.changeYDir = true;
            nextStep.newYDir = newPt.y >= 0 ? CW : CCW;
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
    digitalWrite(ENABLE_PIN, LOW);                   // Enable stepper motors
    digitalWrite(TOP_DIR_PIN, CW);                   // Initialize both steppers
    digitalWrite(BOT_DIR_PIN, CW);
    Timer1.attachInterrupt(drawingInterrupt); // Set interrupt function
    Timer1.start();                           // Start the timer interrupt
}

// Clean up when drawing mode ends
void endDrawing()
{
    Timer1.stop();                     // Stop the timer interrupt
    Timer1.detachInterrupt();          // Remove the interrupt function
    digitalWrite(ENABLE_PIN, HIGH);    // Disable stepper motors
    setRuntimeMode(acceptingCommands); // Return to Accepting Commands
    Serial.println("Ending drawing mode.");
}

void drawingLoop()
{
    Point previous, read, change;
    previous = {0, 0}; // Starts at the origin
    while (continueDrawing)
    {
        while (isFull(&queue))
            delay(10); // Wait 10ms before checking again

        // Wait for a point on serial
        while (Serial.available() < POINTSZ)
        {
        }

        Serial.readBytes((char *)&read, POINTSZ);
        if (read.x == EMERGENCY_STOP)
            break;
        if (read.x == STOP_DRAWING)
        {
            enqueue(&queue, &read);
            return;
        }

        // Queue the change from previous coordinate
        change.x = read.x - previous.x;
        change.y = read.y - previous.y;
        enqueue(&queue, &change);

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