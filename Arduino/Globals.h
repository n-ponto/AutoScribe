#ifndef GLOBALS_H_
#define GLOBALS_H_

typedef struct
{
    uint16_t x;
    uint16_t y;
} Point;

typedef uint8_t Direction;
#define UP 0b1000
#define DOWN 0b0100
#define LEFT 0b0010
#define RIGHT 0b0001

#endif // GLOBALS_H_