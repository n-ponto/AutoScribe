#ifndef RECEIVER_H_
#define RECEIVER_H_

#include "Globals.h"

/****************
 * Commands the drawer can accept over serial:
 * 
 * 1. Set Pen Range
 *      Read two bytes: a byte each for the min angle and max angle.
 * 2. Change Pen Angle
 *      Read one byte for angle to move the pen
 * 3. Move to Coordinate
 *      Read two, two-byte coordinates for x and y respectively.
 * 4. Reset home
 *      Reset the x and y coordinate to zero (no further reading).
 * 5. Draw
 *      Begin accepting 4-byte coordinate pairs until receives 4 null bytes.
 * 6. Step
 *      Begin accepting single bytes indicating which direction to move,
 *      stop when received a null byte
 * 
 ****************/

enum Command
{
    SET_PEN_RANGE,
    CHANGE_PEN_ANGLE,
    MOVE_TO_COORDINATE,
    RESET_HOME,
    DRAW,
    MANUAL_STEP
};

class Receiver
{
public:
    Receiver();
    uint8_t readByte();
    Point readPoint();
};

#endif // RECEIVER_H_