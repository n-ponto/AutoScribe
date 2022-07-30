#ifndef DRAWING_H_
#define DRAWING_H_

#define POINTSZ 4   // Size of the point struct
#define BUFSZ 0x100 // Size in bytes of buffer to allocate for FIFO Queue

/* 
Point Encoding Layout:
need 11 bits to represent positive numbers,
12 bits for twos compliment encoding

|------- coordinate point (16 bits) ----------------|

| 15 14 | 13 12 11 10  9  8  7  6  5  4  3  2  1  0 |
|-------|------------------ 14 bits ----------------|
|-flags-|----------- 2's compliment value ----------|

14 move pen
15 done drawing
*/

// Encodings for commands within the coordiante pairs
#define EMERGENCY_STOP 0x1FFF  // Signal to stop drawing immediately
#define MOVE_PEN (1 << 14)     // Signal to change the pen position
#define STOP_DRAWING (int16_t)(1 << 15) // Signal to stop drawing when this point is reached
#define FLAG_MASK (0b11 << 14)

// Converting from 14 to 16 bit twos compliment
#define NEG_BIT (1 << 13)
#define CONVERT_FTS(x) (((x) & (NEG_BIT)) ? (x) | FLAG_MASK : (x) & ~(FLAG_MASK))
#define CONVERT_STF(x) ((x) &= ~FLAG_MASK)

#define STEP_BOTH 0xFF // Encoding to step both the stepper motors

#define DIFF_SIGN(x, y) ((x ^ y) & NEG_BIT) // Check if one positive and one negative

// 4 byte struct transmitted from computer
typedef struct
{
    int16_t x, y;
} Point;

#endif //DRAWING_H_