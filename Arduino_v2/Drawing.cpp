/*
Contains all code specific to the Manual Control runtime mode.
*/

#include <TimerOne.h>
#include "RuntimeModes.h"
#include "Queue.h"

#define POINTSZ 4
#define BUFSZ   0x400

typedef struct {
    short X, Y;
} Point;

Queue queue;
unsigned char buffer[BUFSZ];

void printPoint(Point *pt)
{
    Serial.print("Received\tX 0x");
    Serial.print(read.X, HEX);
    Serial.print("\tY 0x");
    Serial.println(read.Y, HEX);
}

void start_drawing();
void end_drawing();

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
    while (true)
    {
        while (isFull(queue))
            delay(10); // Wait 10ms before checking again
            
        while(Serial.available() < POINTSZ) {}
        Serial.readBytes((char *)&read, POINTSZ);
        // Check for ending signal
        if (read.X>>8 == 0xFF)
            break;
        
        // Calculate the change in position required from the previous move
        change.X = read.X - current.X;
        change.Y = read.Y - current.Y;
        // Queue the change required
        // Increase the current position
        previous = read;
    }
    end_drawing();
}

void drawing_interrupt()
{
    // Execute the previous command
    // If that's the end of the queded coordinate then unqueue the next coordinate
    // Figure out the next command on the way to the next coordinate
    // Save the next command 
    return;
}

void start_drawing()
{
    Serial.println("Starting drawing mode.");
    queueInit(queue, sizeof(Point), BUFSZ, buffer);
    Timer1.attachInterrupt(drawing_interrupt);
    Timer1.start();
}

void end_drawing()
{
    Timer1.stop();
    Timer1.detachInterrupt();
    setRuntimeMode(accepting_commands);
    Serial.println("Ending drawing mode.");
}