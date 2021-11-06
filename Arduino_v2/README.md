# Arduino

## Overview
This is the code to run on the Arduino. It's responsible for accepting commands
from the computer over serial communication and controlling the two stepper and
one servo motors which make up the drawing machine. By accepting commands from
the computer it's able to draw images using a pen.

---

# Technical Details

## Runtime Modes
These influence how the Arduino reads information from the serial and
acts on that information.

**1. [Accepting Commands](#accepting-commands-mode)** - listens for commands over serial.

**2. [Manual Control](#manual-control-mode)** - used to manually control the movement of drawing machine in real time

**3. [Drawing](#drawing-mode)** - used to draw an image by interpreting instructions over serial



## Accepting Commands mode
Listens to serial connection for one byte commands from the list of commands below. Generally, if the command can be completed quickly or requires reading a predetermined amount of data from serial, it is executed in the Accepting Commands runtime mode before listening for the next command. If the command is to switch commands, then the Arduino will switch remain in the other runtime mode until receiving a signal to return to this mode.

| Command | Description |
| ----------- | ----------- |
| SET_PEN_RANGE | Used to define the top and bottom boundaries of the pen's movement. Will listen for two more bytes, one byte each for the minimum and maximum angle for the pens movement.  |
| CHANGE_PEN_ANGLE | Used to immediately change the current angle of the pen. This can be useful when trying to determine the optimal pen range. It will listen for one more byte for the angle to move the pen (i.e. the servo motor). | 
| MOVE_TO_COORDINATE | Used to immediately change the current location of the pen. It will listent for 4 total more bytes, two 2B integers for the x and y coordinates. | 
| RESET_HOME | Used to reset the home value of the steppers. When this command is executed the current location of the end effector will be considered the origin. |
| ENTER_DRAW_MODE | Will go into Drawing mode.  |
| ENTER_MANUAL_CONTROL_MODE | Will go into Manual Control mode. |
| SET_STEPPER_DELAY | Sets the delay between signals to the steppers. This value is directly related to the movement speed of the steppers. Should be calibrated to optimize between speed and minimal shaking. |

## Manual Control mode
After entering this mode, it will continuously listen for single byte packets indicating how to move. These instructions will either move the pen up/down, or will indicate how to move the steppers. If this mode receives the instruction to stop, it will return to the accepting commands mode.

## Drawing mode 
After entering this mode, it will continuously listen for 4 byte packets, where each packet will contain a set of two 2-byte coordinates.

The maximum coordinate range of the drawing machine can be represented using only 11-bits, meaning that each 16-bit coordinate dimension has 5 bits left over. These extra bits are used to encode additional information: pen up, pen down, and to end drawing mode.

