#ifndef GLOBALS_H_
#define GLOBALS_H_

typedef struct
{
    uint16_t x;
    uint16_t y;
} Point;

// Directions to controll the steppers
// NOTE valid direction combos (HEX): 1, 2, 4, 5, 6, 8, 9, A
typedef uint8_t Direction;
#define UP    0x08
#define DOWN  0x04
#define LEFT  0x02
#define RIGHT 0x01

// Commands while in manual step mode
#define MAN_PEN_UP   0x10
#define MAN_PEN_DOWN 0x11
#define MAN_END      0x00

// Commands while in draw mode
#define DRAW_PEN_UP   0b10000
#define DRAW_PEN_DOWN 0b01000
#define DRAW_END      0b11111
#define COORD_MASK    0b0000011111111111

#endif // GLOBALS_H_