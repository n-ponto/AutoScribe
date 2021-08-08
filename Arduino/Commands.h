#ifndef COMMANDS_H_
#define COMMANDS_H_

#define HANDLE(x) functions[x]()

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
 * 6. Manual step
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

void setPenRange();
void changePenAngle();
void resetHome();
void draw();
void manualStep();

void (*functions[])() = {
    setPenRange,
    changePenAngle,
    resetHome,
    draw,
    manualStep};

#endif // COMMANDS_H_