/*
Contains all code specific to the Drawing runtime mode.
*/

#ifndef TESTING_MOCK
#include <TimerOne.h>
#include "RuntimeModes.h"
#else
#include "../CTests/DrawingMocks.h"
#endif //TESTING_MOCK
#include "Queue.h"
#include "Stepper.h"

#define POINTSZ 4   // Size of the point struct
#define BUFSZ 0x400 // Size in bytes of buffer to allocate for FIFO Queue

#define EMERGENCY_STOP 0xFFFF // Signal to stop drawing immediately
#define STOP_DRAWING 0xF0     // Signal to stop drawing when point dequeued

#define STEP_BOTH 0xFF // Encoding to step both the stepper motors

#define DIFF_SIGN(x, y) ((x ^ y) >> 15) // Check if one positive and one negative

// 4 byte struct transmitted from computer
typedef struct
{
    int16_t x, y;
} Point;

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
    int8_t x_chnage, y_change;
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
Queue queue;          // FIFO queue for the coordinates received
volatile bool continueDrawing; // Set by interrupt to leave drawing mode when complete

void printPoint(Point *pt)
{
    Serial.print("Received\tX 0x");
    Serial.print(pt->x, HEX);
    Serial.print("\tY 0x");
    Serial.println(pt->y, HEX);
}

// Set up the drawing state with new dequeued point
void initdrawState(Point *pt)
{
    bool back = pt->x < 0;
    bool down = pt->y < 0;
    int16_t absx, absy;
    absx = back ? -1 * pt->x : pt->x;
    absy = down ? -1 * pt->y : pt->y;
    drawState.swapxy = absy > absx;

    // Check if need to switch x and y
    if (drawState.swapxy)
    {
        int16_t save = absx;
        absx = absy;
        absy = save;
    }

    // int16_t m, slopeError, c;
    drawState.m = 2 * absy;
    drawState.slopeError = drawState.m - absx;
    drawState.c = drawState.slopeError - absx;
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
    if (nextStep.newYDir)
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
    else
    {
        digitalWrite(nextStep.stepPin, nextStep.writeVal);
    }

    // If wrote high to either stepper then write low next and return
    if (nextStep.writeVal)
    {
        // Want to write to the same stepPin
        nextStep.writeVal = LOW;
        return;
    }

    /*************** CHECK IF REACHED COORDINATE ***************/
    if (drawState.i >= drawState.absx)
    {
        // Dequeue a new point
        Point newPt;
        dequeue(&queue, &newPt);
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
        initdrawState(&newPt);
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
}

// Set up variables when drawing mode first starts
void start_drawing()
{
    Serial.println("Starting drawing mode.");
    queueInit(&queue, buffer, sizeof(Point), BUFSZ); // Initialize queue
    continueDrawing = true;                          // Loop drawing mode
    nextStep = {0, 0, false, false, 0, 0};           // Clear next motor commands
    drawState = {0, 0, 0, 0, 0};                     // Clear drawing state
    digitalWrite(ENABLE_PIN, LOW);                   // Enable stepper motors
    Timer1.attachInterrupt(drawingInterrupt);        // Set interrupt function
    Timer1.start();                                  // Start the timer interrupt
}

// Clean up when drawing mode ends
void end_drawing()
{
    Timer1.stop();                     // Stop the timer interrupt
    Timer1.detachInterrupt();          // Remove the interrupt function
    digitalWrite(ENABLE_PIN, HIGH);    // Disable stepper motors
    setRuntimeMode(acceptingCommands); // Return to Accepting Commands
    Serial.println("Ending drawing mode.");
}

// The main thread for reading and queueing instructions from serial
void drawing()
{
    start_drawing();
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
        if (read.x >> 8 == EMERGENCY_STOP)
            break;

        // Queue the change from previous coordinate
        change.x = read.x - previous.x;
        change.y = read.y - previous.y;
        enqueue(&queue, &change);

        previous = read; // Update the previous position
    }
    end_drawing();
}