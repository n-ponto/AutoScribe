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

#define POINTSZ 4
#define BUFSZ 0x400
#define NO_OP

typedef struct
{
    int16_t x, y;
} Point;

Queue queue;
unsigned char buffer[BUFSZ];

void printPoint(Point *pt)
{
    Serial.print("Received\tX 0x");
    Serial.print(pt->x, HEX);
    Serial.print("\tY 0x");
    Serial.println(pt->y, HEX);
}

void start_drawing();
void end_drawing();

// Value refrenced by main thread and interrupt to coordinate leaving
// the drawing runtime mode
volatile bool continue_drawing;

// The main thread for reading and queueing instructions from serial
void drawing()
{
    // Read 4 bytes from serial corresponding to a coordinate pair
    // Proccess the instructions from the coordinate pair and
    // queue the instruction to be read by the interrupt

    // If the queue is full then stop reading
    start_drawing();
    Point previous, read, change;
    // Set the starting point to the origin
    previous = {0, 0};
    while (continue_drawing)
    {
        while (isFull(&queue))
            delay(10); // Wait 10ms before checking again

        // Wait for a point on serial
        while (Serial.available() < POINTSZ)
            NO_OP;

        Serial.readBytes((char *)&read, POINTSZ);
        // Check for emergency ending signal
        if (read.x >> 8 == 0xFFFF)
            break;

        // Calculate the change in position required from the previous move
        change.x = read.x - previous.x;
        change.y = read.y - previous.y;
        // Queue the change required
        enqueue(&queue, &change);
        // Increase the current position
        previous = read;
    }
    end_drawing();
}

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

#define STOP_DRAWING 0xF0  // Encoding on a queued point to stop drawing

#define STEP_BOTH 0xFF
#define DIFF_SIGN(x, y) ((x ^ y) >> 15)

struct ns
{
    uint8_t stepPin, write_value;
    bool changeXDir, changeYDir;
    uint8_t newXDir, newYDir;
} nextStep;


void drawingInterrupt()
{
    /*************** STEP MOTORS ***************/
    // Change stepper directions
    if (nextStep.changeXDir)
    {
        digitalWrite(DIR_TOP, nextStep.newXDir);
        nextStep.changeXDir = false;
    }
    if (nextStep.newYDir)
    {
        digitalWrite(DIR_BOT, nextStep.newYDir);
        nextStep.changeYDir = false;
    }

    // Step the motors
    if (nextStep.stepPin == STEP_BOTH)
    {
        digitalWrite(STEP_TOP, nextStep.write_value);
        digitalWrite(STEP_BOT, nextStep.write_value);
    }
    else
    {
        digitalWrite(nextStep.stepPin, nextStep.write_value);
    }

    // If wrote high to either stepper then write low next and return
    if (nextStep.write_value)
    {
        // Want to write to the same stepPin
        nextStep.write_value = LOW;
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
    nextStep.write_value = HIGH;
    if (drawState.slopeError >= 0)
    {
        nextStep.stepPin = STEP_BOTH;
        drawState.slopeError += drawState.c;
    }
    else
    {
        nextStep.stepPin = drawState.swapxy ? STEP_BOT : STEP_TOP;
        drawState.slopeError += drawState.m;
    }
}

void start_drawing()
{
    Serial.println("Starting drawing mode.");
    queueInit(&queue, buffer, sizeof(Point), BUFSZ); // Initialize queue
    continue_drawing = true;
    // motorX = motorY = {false, 0, false, 0}; // Clear motor commands
    drawState = {0, 0, 0, 0, 0};            // Clear drawing state
    // enable steppers
    Timer1.attachInterrupt(drawingInterrupt); // Set interrupt function
    Timer1.start();                            // Start the timer interrupt
}

void end_drawing()
{
    Timer1.stop();
    Timer1.detachInterrupt();
    // disable steppers
    setRuntimeMode(acceptingCommands);
    Serial.println("Ending drawing mode.");
}