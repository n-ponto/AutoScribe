/*
Contains all code specific to the Manual Control runtime mode.
*/

#include <TimerOne.h>
// #include "RuntimeModes.h"

#define DRAW_END

#define POINTSZ 4
#define PTADD(a, b) (a.X += b.X; a.Y += b.Y)

typedef struct {
    unsigned short X, Y;
} Point;

void printBytes(char buf[4])
{
    Serial.print("Received ");
    for (int i = 0; i < 4; i++)
    {
        Serial.print(buf[i], HEX);
    }
    Serial.println();
}

void init_drawing();
void end_drawing();

void drawing()
{
    // Read 4 bytes from serial corresponding to a coordinate pair
    // Proccess the instructions from the coordinate pair and 
    // queue the instruction to be read by the interrupt
    
    // If the queue is full then stop reading
    init_drawing();
    Point read, current;
    current = {0, 0};
    char buf[POINTSZ];
    while (true)
    {
        while(Serial.available() < 4) {}
        Serial.readBytes(buf, POINTSZ);
        printBytes(buf);
        if (buf[0] & 0xFF)
            break;
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

void init_drawing()
{
    Serial.println("Starting drawing mode.");
    Timer1.attachInterrupt(drawing_interrupt);
    Timer1.start();
}

void end_drawing()
{
    Timer1.stop();
    Timer1.detachInterrupt();
    // runtime_mode = accepting_commands;
    Serial.println("Ending drawing mode.");
}