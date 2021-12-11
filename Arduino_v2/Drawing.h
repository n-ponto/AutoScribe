#ifndef DRAWING_H_
#define DRAWING_H_

#define POINTSZ 4   // Size of the point struct
#define BUFSZ 0x400 // Size in bytes of buffer to allocate for FIFO Queue

/* 
Point Encoding Layout:
need 11 bits to represent positive numbers,
12 bits for twos compliment encoding

|------- coordinate point (16 bits) ----------------|

| 15 14 13 12 | 11 10  9  8  7  6  5  4  3  2  1  0 |
|-- 4 bits ---|-------------- 12 bits --------------|
|-- flags ----|--------- 2's compliment value ------|

12 change pen
13 pen up (down if change pen and 0)
14 stop drawing
15 unused
*/

// Encodings for commands within the coordiante pairs
#define EMERGENCY_STOP 0x7FFF // Signal to stop drawing immediately
#define FLAG_MASK (0b1111 << 12)
#define MOVE_PEN (1 << 12)   // Signal to change the pen position
#define PEN_UP (1 << 13)       // Indicates how to change the pen position
#define STOP_DRAWING (1 << 14) // Signal to stop drawing when this point is reached

// Converting from 11 to 16 bit twos compliment
#define NEG_BIT (1 << 11)
#define CONVERT_ETS(x) (((x) &(NEG_BIT)) ? (x) | FLAG_MASK : (x) & ~(FLAG_MASK))
#define CONVERT_STE(x) ((x) &= ~FLAG_MASK)

#define STEP_BOTH 0xFF // Encoding to step both the stepper motors

#define DIFF_SIGN(x, y) ((x ^ y) & NEG_BIT) // Check if one positive and one negative

// 4 byte struct transmitted from computer
typedef struct
{
    int16_t x, y;
} Point;

#endif //DRAWING_H_