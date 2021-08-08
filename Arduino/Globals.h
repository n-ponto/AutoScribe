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

#define PEN_UP   0b1100
#define PEN_DOWN 0b0011
#define END      0b0000

#endif // GLOBALS_H_