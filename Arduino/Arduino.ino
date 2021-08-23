#include "Globals.h"
#include "Commands.h"
#include "Director.h"
#include "Receiver.h"
#include "Pen.h"
#include "InstructionQueue.h"

Director *director;
Receiver *receiver;
Pen *pen;

void setup()
{
    receiver = new Receiver();
    director = new Director();
    pen = new Pen();
}

void loop()
{
    Command command = (Command)receiver->readByte();
    HANDLE(command);
}

void setPenRange()
{
    Serial.println("setPenRange");
    Angle min, max;
    min = receiver->readByte();
    max = receiver->readByte();
    pen->setUpAngle(min);
    pen->setDownAngle(max);
}

void changePenAngle()
{
    Serial.println("changePenAngle");
    Angle angle = receiver->readByte();
    pen->goTo(angle);
}

void moveToCoordinate()
{
    Serial.println("Not done.");
}

void resetHome()
{
    Serial.println("Resetting home...");
    director->resetHome();
}

// Only takes 11bits to represent a coordinate dimension
// Leaves 5bits in each 2byte coordinate
void draw()
{
    Serial.println("Entering drawing mode");
    Point pt;
    director->enable();
    while(true)
    {
        pt = receiver->readPoint();
        uint8_t switchVal = pt.x>>11;
        Serial.print("switchVal: ");
        Serial.println(switchVal, BIN);
        switch(switchVal)
        {
            case(0): break;
            case(DRAW_PEN_UP): 
                pen->up();
                delay(100);
                Serial.println("UP"); 
                break;
            case(DRAW_PEN_DOWN): 
                pen->down(); 
                delay(100);
                Serial.println("DOWN"); 
                    break;
            case(DRAW_END): 
                Serial.println("Ending drawing mode");
                director->disable();
                return;
        }
        pt.x &= COORD_MASK;
        pt.y &= COORD_MASK;
        Serial.print("Read point: (");
        Serial.print(pt.x);
        Serial.print(", ");
        Serial.print(pt.y);
        Serial.println(")");
        director->travel(pt);
    }
}

InstructionQueue *instructionQueue;

void draw()
{
    Serial.println("Entering drawing mode");
    Point pt;

    // Create a new head instruction queue
    // The head queue shouldn't be touched for the rest of this
    instructionQueue = new InstructionQueue;

    // Create a local queue and initialize to the head
    InstructionQueue *localQueue = instructionQueue;
    uint8_t *queue = instructionQueue->queue;
    uint8_t queueIndex = 0;
    // director->enable();
    Point prevPt = {0, 0};

    // Read from the serial port and populate queue with instructions
    while (true)
    {
        pt = receiver->readPoint();
        uint8_t switchVal = pt.x>>11;
        
        // Interpret point from serial
        switch(switchVal)
        {
            // No command means just a coordinate
            case(0):
                break;
            case(DRAW_PEN_UP):
                // Add pen up instruction to the queue
                queue[queueIndex++] = PEN_UP_INSTRUCTION;
                break;
            case(DRAW_PEN_DOWN):
                // Add pen down instruction to the queue
                queue[queueIndex++] = PEN_DOWN_INSTRUCTION;
                queueIndex++;
                break;
            case(DRAW_END): 
                director->disable();
                return;
            
            // Calculate the local movement required
            uint16_t relativeX, relativeY;
            relativeX = pt.x - prevPt.x;
            relativeY = pt.y - prevPt.y;

            // Add the local movement to the instruction queue
            if (relativeX < 0x40 && relativeY < 0x100)  // Fit in one byte
            {
                // Coordinates pair can fit in two bytes
                queue[queueIndex++] = TWO_BYTE + relativeX;
                queue[queueIndex++] = relativeY;
            }
            else
            {
                // Coordinate pair requires 4 bytes
                queue[queueIndex+=2] = relativeX;
                queue[queueIndex+=2] = relativeY;
            }
            // Update the previous point
            prevPt = pt;
        }

        // Handle queue overflow
        /* For now, just make a new queue if there's less than 4 bytes which ensures that 
        we'll have space for the largest element (4 byte coordinate pair), but could 
        mean we're wasting 3 bytes per queue. */
        if (QUEUE_SIZE < queueIndex + 4)  // can fit another 4 bytes?
        {
            // Create a new queue and create a link
            instructionQueue->next = new InstructionQueue;
            instructionQueue = instructionQueue->next;
            queue = instructionQueue->queue;
            
        }

    }
}

// Enters manual step mode:
// Takes a direction (as defined by Direction struct)
// If the byte is null then stop
void manualStep()
{
    Serial.println("Entering manual step mode");
    uint8_t b;
    director->enable();
    while(true)
    {
        b = receiver->readByte();
        switch(b)
        {
            case(MAN_END): 
                Serial.println("Ending manual step mode"); 
                director->disable();
                return;
            case(MAN_PEN_UP): pen->up(); break;
            case(MAN_PEN_DOWN): pen->down(); break;
            default:
                director->move(b, 1);
                break;
        }
    }
}

void setStepperDelay()
{
    uint16_t b = receiver->readPair();
    director->setDelay(b);
    Serial.print("Set stepper delay to ");
    Serial.println(b, DEC);
}


void timerInterrupt()
{
    
}